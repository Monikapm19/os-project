#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include "monitor_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("OS Project");
MODULE_DESCRIPTION("Container Memory Monitor (Fixed)");

#define DEVICE_NAME "container_monitor"

/* ===== Task Entry ===== */
struct task_entry {
    pid_t pid;
    int soft;
    int hard;
    struct list_head list;
};

static LIST_HEAD(task_list);
static dev_t dev_no;
static struct cdev monitor_cdev;

static struct task_struct *monitor_thread;

/* ===== RSS in MB ===== */
static unsigned long get_rss_mb(struct task_struct *task) {
    struct mm_struct *mm = task->mm;
    if (!mm) return 0;

    return (get_mm_rss(mm) << (PAGE_SHIFT - 10)) / 1024;
}

/* ===== IOCTL ===== */
static long monitor_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {

    struct task_entry *entry;
    struct limits lim;

    switch (cmd) {

        case IOCTL_REGISTER_PID:
            entry = kmalloc(sizeof(*entry), GFP_KERNEL);
            entry->pid = (pid_t)arg;
            entry->soft = 0;
            entry->hard = 0;
            INIT_LIST_HEAD(&entry->list);
            list_add(&entry->list, &task_list);

            printk(KERN_INFO "[monitor] Registered PID %d\n", entry->pid);
            break;

        case IOCTL_SET_LIMITS:
            if (copy_from_user(&lim, (void __user *)arg, sizeof(lim)))
                return -EFAULT;

            list_for_each_entry(entry, &task_list, list) {
                if (entry->pid == lim.pid) {
                    entry->soft = lim.soft_limit_mb;
                    entry->hard = lim.hard_limit_mb;

                    printk(KERN_INFO "[monitor] Limits set PID %d soft=%d hard=%d\n",
                           lim.pid, lim.soft_limit_mb, lim.hard_limit_mb);
                }
            }
            break;
    }

    return 0;
}

/* ===== MONITOR THREAD ===== */
static int monitor_fn(void *data) {

    struct task_entry *entry;
    struct task_struct *task;

    while (!kthread_should_stop()) {

        list_for_each_entry(entry, &task_list, list) {

            task = pid_task(find_vpid(entry->pid), PIDTYPE_PID);
            if (!task)
                continue;

            unsigned long rss = get_rss_mb(task);

            if (entry->soft && rss > entry->soft) {
                printk(KERN_WARNING "[monitor] Soft limit exceeded PID %d (%lu MB)\n",
                       entry->pid, rss);
            }

            if (entry->hard && rss > entry->hard) {
                printk(KERN_ALERT "[monitor] HARD LIMIT exceeded PID %d -> KILL\n",
                       entry->pid);

                send_sig(SIGKILL, task, 0);
            }
        }

        msleep(2000);
    }

    return 0;
}

/* ===== FILE OPS ===== */
static struct file_operations fops = {
    .unlocked_ioctl = monitor_ioctl,
};

/* ===== INIT ===== */
static int __init monitor_init(void) {

    alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);

    cdev_init(&monitor_cdev, &fops);
    cdev_add(&monitor_cdev, dev_no, 1);

    /* start kernel thread */
    monitor_thread = kthread_run(monitor_fn, NULL, "monitor_thread");

    printk(KERN_INFO "[monitor] module loaded\n");
    return 0;
}

/* ===== EXIT ===== */
static void __exit monitor_exit(void) {

    struct task_entry *entry, *tmp;

    if (monitor_thread)
        kthread_stop(monitor_thread);

    list_for_each_entry_safe(entry, tmp, &task_list, list) {
        list_del(&entry->list);
        kfree(entry);
    }

    cdev_del(&monitor_cdev);
    unregister_chrdev_region(dev_no, 1);

    printk(KERN_INFO "[monitor] module unloaded\n");
}

module_init(monitor_init);
module_exit(monitor_exit);
