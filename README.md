# Multi-Container Runtime (Linux in C + Kernel Module)

---

## Team Information
- Student 1: Monika Pm
- Student 2: (Add name + SRN)

---

## 1. Project Overview

This project implements a lightweight multi-container runtime in C along with a Linux Kernel Module for memory monitoring and process tracking.

It supports:
- Multiple container execution
- Namespace-based isolation
- Supervisor-based lifecycle management
- IPC-based CLI communication
- Kernel-level memory monitoring
- Scheduling experiments using CPU workloads

---

## 2. Architecture

### Supervisor (engine)
- Long-running process
- Manages all containers
- Handles CLI commands via IPC
- Maintains container metadata in memory

### Containers
- Created using fork() + exec()
- Isolated using:
  - PID namespace
  - Mount namespace
  - chroot filesystem isolation
- Each container has its own rootfs copy

### Kernel Module (monitor)
- Tracks process memory usage
- Enforces:
  - Soft limit → warning
  - Hard limit → kill process
- Registers processes via ioctl

---

## 3. Build Instructions

```bash
make
4. Load Kernel Module
  sudo insmod monitor.ko
  lsmod | grep monitor 
5. Start Supervisor
sudo ./engine supervisor ./rootfs-base
6. Start Containers
sudo ./engine start alpha ./rootfs-alpha /bin/sleep 100
sudo ./engine start beta ./rootfs-beta /bin/sleep 100
7. List Containers
sudo ./engine ps
8. Stop Containers
sudo ./engine stop alpha
sudo ./engine stop beta
9. Kernel Module Cleanup
sudo rmmod monitor
sudo dmesg | tail -20
10. Scheduler Experiment (Task 5.2)
CPU Workload Script (cpu.sh)
Make executable:
chmod +x cpu.sh
Copy into containers:
cp cpu.sh rootfs-alpha/
cp cpu.sh rootfs-beta/

11. Clean Teardown
All containers are properly terminated
Supervisor reaps child processes
No zombie processes remain
Kernel module is safely unloaded
All resources are freed

Observation:
Two CPU-intensive containers executed simultaneously
High CPU usage observed using top
Processes competed for CPU time



12. Key Concepts Used
Linux namespaces (PID, Mount)
chroot isolation
fork/exec process model
IPC (FIFO / socket communication)
Producer-consumer logging model
Kernel module programming
Process scheduling (CFS)
Memory monitoring in kernel space

Conclusion:
This project demonstrates a simplified container runtime built using Linux system programming concepts. It integrates process isolation, scheduling experiments, IPC communication, and kernel-level memory monitoring to simulate real container orchestration behavior.
