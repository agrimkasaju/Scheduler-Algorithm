#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__
#define MAX_PROCESSES 100

// Structure to represent a memory partition (number, size, and status)
typedef struct {
    unsigned int num;
    unsigned int size;
    int occupied; // PID of the process using the partition, -1 if free
} partition_t;

// Structure for Process Control Block (PCB) details
typedef struct {
    int pid;                 // Process ID
    int arrival_time;        // Time at which the process arrives
    int total_cpu_time;      // Total CPU time required by the process
    int remaining_time;      // Remaining CPU time (for preemption or tracking progress)
    int io_frequency;        // Frequency of I/O events (e.g., every X milliseconds)
    int io_duration;         // Duration of each I/O event
    char current_state[10];  // Current state of the process (e.g., "NEW", "READY", "RUNNING", "WAITING", "TERMINATED")
    char next_state[10];     // Next state after transition
    int partition_num;       // Partition number where the process resides in memory
    int memory_size;         // Memory size required by the process
    int completion_time;    // Time when process is terminated
    int total_burst_time;   // Total CPU burst time
    int total_wait_time;    // Total time spent in ready queue
    int turnaround_time;    // TAT = completion_time - arrival_time
    int priority;
} pcb_t;

typedef struct node {
    int process;
    struct node *next;
}node_t;

typedef struct {
    node_t *front;
    node_t *rear;
    int size;
}ready_queue_t;

typedef struct link{
  int data;
  int process;
  struct link *next;
}link_t;

typedef struct {
    link_t *front;
    link_t *rear;
    int size;
}waiting_queue_t;

// Define the structure for a log entry
typedef struct {
    int time_of_transition;
    int pid;
    char old_state[20];
    char new_state[20];
} log_entry_t;

#define NUM_PARTITIONS 6

// Global variables for PCB table, next available PID, file buffer, and process counter
pcb_t pcb_table[MAX_PROCESSES];
char file_buffer[50];
int process_counter = 0;

// Initializes memory partitions as free with default sizes
void initpartition(partition_t partition[]);


// Finds the first available partition large enough for a program
// int getpartition_size(partition_t partition[], int size);

// Return the partition number
int getpartition_num(partition_t partition[], int size);

// Initializes the PCB for a new process in a specified partition
// void initpcb(partition_t partition[], int pid, int cpu, int io_time, int counter, int partition_num);

// Updates the PCB with new process details and memory partition information
// void update_pcb(partition_t partition[], int pid, int cpu, int io_time, int counter, int partition_num, external_file_t files[], int process_counter);

// Logs the current system status (active PCBs and partitions) to a file
// void log_system_status(partition_t partition[], int IRET, int n);

// Reads and executes a program's instructions from a file
// int readProgram(const char *filename_r);

#endif