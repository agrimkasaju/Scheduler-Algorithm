# Scheduler-Algorithm

This project simulates **FCFS** (First-Come, First-Served) and **priority-based** scheduling algorithms.

---

## Part 1: FCFS and EP Simulation

### Example Compilation Scripts
- `test_1.txt`, `test_2.txt`

### Manual Compilation
You can compile and run the source file manually with the following commands:

```bash
g++ interrupts_101280101_101219454.c -o sim
./sim input_data_n.txt FCFS

## Part 2: Multithreaded Process Synchronization

This section demonstrates **concurrent producer–consumer synchronization** using **POSIX threads**, **shared memory**, and **semaphores**.

### Compilation
```bash
gcc -o code Part2_n_101280101_101219454.c -lpthread

