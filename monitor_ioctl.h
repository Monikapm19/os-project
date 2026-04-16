#ifndef MONITOR_IOCTL_H
#define MONITOR_IOCTL_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define MAGIC 'M'

#define IOCTL_REGISTER_PID _IOW(MAGIC, 1, int)
#define IOCTL_SET_LIMITS _IOW(MAGIC, 2, struct limits)

struct limits {
    pid_t pid;
    int soft_limit_mb;
    int hard_limit_mb;
};

#endif
