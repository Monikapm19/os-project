#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "monitor_ioctl.h"

#define STACK_SIZE (1024 * 1024)
#define FIFO_PATH "/tmp/engine_fifo"

char container_stack[STACK_SIZE];

typedef struct {
    char id[32];
    pid_t pid;
    char state[16];
} Container;

Container containers[10];
int container_count = 0;

/* ================= CONTAINER ================= */
int container_main(void *arg) {
    char **argv = (char **)arg;

    sethostname("container", 9);

    if (chroot(argv[0]) != 0) {
        perror("chroot failed");
        exit(1);
    }

    chdir("/");

    if (mount("proc", "/proc", "proc", 0, NULL) != 0) {
        perror("mount proc failed");
        exit(1);
    }

    execvp(argv[1], &argv[1]);
    perror("exec failed");
    return 1;
}

/* ================= START CONTAINER ================= */
void start_container(char *id, char *rootfs, char **cmd) {

    int pipefd[2];
    pipe(pipefd);

    pid_t pid = clone(container_main,
                      container_stack + STACK_SIZE,
                      CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD,
                      cmd);

    if (pid < 0) {
        perror("clone failed");
        return;
    }

    /* CHILD */
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        return;
    }

    /* PARENT */
    close(pipefd[1]);

    char logfile[64];
    snprintf(logfile, sizeof(logfile), "%s.log", id);

    int logfd = open(logfile, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (logfd < 0) {
        perror("log open failed");
        return;
    }

    /* ================= KERNEL MONITOR INTEGRATION ================= */
    int devfd = open("/dev/container_monitor", O_RDWR);
    if (devfd < 0) {
        perror("monitor device open failed");
    } else {
        ioctl(devfd, IOCTL_REGISTER_PID, pid);

        struct limits lim;
        lim.pid = pid;
        lim.soft_limit_mb = 40;
        lim.hard_limit_mb = 64;

        ioctl(devfd, IOCTL_SET_LIMITS, &lim);

        close(devfd);
    }

    /* store metadata */
    snprintf(containers[container_count].id, 32, "%s", id);
    containers[container_count].pid = pid;
    strcpy(containers[container_count].state, "running");
    container_count++;

    printf("[Supervisor] Started %s (PID %d)\n", id, pid);

    /* ================= LOGGING ================= */
    char buf[256];
    int n;

    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        write(logfd, buf, n);
    }

    close(pipefd[0]);
    close(logfd);
}

/* ================= SUPERVISOR / CLI ================= */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s supervisor|start|ps\n", argv[0]);
        return 1;
    }

    /* ================= SUPERVISOR ================= */
    if (strcmp(argv[1], "supervisor") == 0) {

        printf("[Supervisor] Running...\n");

        mkfifo(FIFO_PATH, 0666);
        int fd = open(FIFO_PATH, O_RDONLY);

        char buffer[1024] = {0};
        char temp[256];

        while (1) {

            int n = read(fd, temp, sizeof(temp) - 1);
            if (n <= 0) continue;

            temp[n] = '\0';
            strcat(buffer, temp);

            char *line;

            while ((line = strchr(buffer, '\n')) != NULL) {

                *line = '\0';

                if (strlen(buffer) > 0) {

                    printf("[Supervisor] Received: %s\n", buffer);

                    char *args[10];
                    int i = 0;

                    char *token = strtok(buffer, " ");
                    while (token != NULL && i < 9) {
                        args[i++] = token;
                        token = strtok(NULL, " ");
                    }
                    args[i] = NULL;

                    if (args[0] == NULL) {
                        // ignore
                    }
                    else if (strcmp(args[0], "start") == 0 && i >= 4) {
                        start_container(args[1], args[2], &args[2]);
                    }
                    else if (strcmp(args[0], "ps") == 0) {
                        printf("ID\tPID\tSTATE\n");
                        for (int j = 0; j < container_count; j++) {
                            printf("%s\t%d\t%s\n",
                                   containers[j].id,
                                   containers[j].pid,
                                   containers[j].state);
                        }
                    }
                }

                memmove(buffer, line + 1, strlen(line + 1) + 1);
            }
        }
    }

    /* ================= CLI ================= */
    else {

        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd < 0) {
            perror("FIFO open failed");
            return 1;
        }

        for (int i = 1; i < argc; i++) {
            write(fd, argv[i], strlen(argv[i]));
            write(fd, " ", 1);
        }

        write(fd, "\n", 1);
        close(fd);
    }

    return 0;
}
