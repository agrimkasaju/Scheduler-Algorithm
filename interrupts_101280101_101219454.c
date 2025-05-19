#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "interrupts_101280101_101219454.h"
#include <assert.h>

void initpartition(partition_t partition[])
{
    unsigned int sizes[] = {40, 25, 15, 10, 8, 2};
    for (int i = 0; i < 6; i++)
    {
        partition[i].num = i + 1;
        partition[i].size = sizes[i];
        partition[i].occupied = -1;
    }
}

int getpartition_num(partition_t partition[], int size)
{
    int index = 0;
    for (int i = 0; i < 6; i++)
    {
        if (size == partition[i].size)
        {
            index = i;
        }
    }
    return (index + 1);
}

int getpartition_size(partition_t partition[], int size, int pid)
{
    for (int i = 5; i >= 0; i--)
    {
        if (partition[i].occupied == -1 && (partition[i].size >= size))
        {
            partition[i].occupied = pid; // Assign partition to process
            return partition[i].size;    // Return partition number
        }
    }
    return -1; // No suitable partition available
}

void initpcb(partition_t partition[], int pid, int cpu, int io_freq, int io_time, int counter, int partition_num)
{
    pcb_table[counter].pid = pid;
    pcb_table[counter].io_frequency = io_freq;
    pcb_table[counter].io_duration = io_time;
    strcpy(pcb_table[counter].current_state, "new");
    strcpy(pcb_table[counter].next_state, "ready");
    pcb_table[counter].remaining_time = cpu;
    int i = partition_num - 1;
    partition[i].occupied = -1;
}

void generate_input_data(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    // Define the process data as integers
    int processes[][6] = {
        {15, 10, 0, 25, 11, 3},
        {12, 1, 0, 50, 10, 5},
        {1, 10, 0, 100, 25, 25},
        {2, 1, 5, 20, 10, 2},
        {3, 7, 14, 5, 2, 10}};

    // Number of processes
    size_t num_processes = sizeof(processes) / sizeof(processes[0]);

    // Write the process data to the file
    for (size_t i = 0; i < num_processes; i++)
    {
        fprintf(file, "%d, %d, %d, %d, %d, %d\n",
                processes[i][0], processes[i][1], processes[i][2],
                processes[i][3], processes[i][4], processes[i][5]);
    }

    fclose(file);

    printf("Input data written to %s successfully.\n", filename);
}

void generate_priority_input_data(const char *filename)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    // Define the process data as integers
    int processes[][7] = {
        {1, 10, 0, 100, 25, 25, 2},
        {12, 1, 0, 50, 10, 5, 3},
        {15, 10, 0, 25, 11, 3, 1},
        {2, 1, 5, 20, 10, 2, 5},
        {3, 7, 14, 5, 2, 10, 4}};

    // Number of processes
    size_t num_processes = sizeof(processes) / sizeof(processes[0]);

    // Write the process data to the file
    for (size_t i = 0; i < num_processes; i++)
    {
        fprintf(file, "%d, %d, %d, %d, %d, %d, %d\n",
                processes[i][0], processes[i][1], processes[i][2],
                processes[i][3], processes[i][4], processes[i][5], processes[i][6]);
    }

    fclose(file);

    printf("Input data written to %s successfully.\n", filename);
}

// Function to generate random data for non-priority scheduling
void generate_non_priority_file(int file_num)
{
    char filename[50];
    sprintf(filename, "input_data_%d.txt", file_num);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file %s.\n", filename);
        return;
    }

    for (int j = 0; j < 5; j++)
    { // Each file has 5 processes
        int pid = (file_num * 5) + j + 1;
        int memory_size = (rand() % 20) + 1;      // Memory size between 1 and 20
        int arrival_time = rand() % 50;           // Arrival time between 0 and 49
        int total_cpu_time = (rand() % 100) + 10; // CPU time between 10 and 109
        int io_frequency = (rand() % 25) + 5;     // I/O frequency between 5 and 29
        int io_duration = (rand() % 10) + 1;      // I/O duration between 1 and 10
        fprintf(file, "%d, %d, %d, %d, %d, %d\n",
                pid, memory_size, arrival_time, total_cpu_time, io_frequency, io_duration);
    }

    fclose(file);
    printf("Non-priority input data written to %s successfully.\n", filename);
}

// Function to generate random data for priority scheduling
void generate_priority_file(int file_num)
{
    char filename[50];
    sprintf(filename, "input_data_%d.txt", file_num);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file %s.\n", filename);
        return;
    }

    for (int j = 0; j < 5; j++)
    { // Each file has 5 processes
        int pid = (file_num * 5) + j + 1;
        int memory_size = (rand() % 20) + 1;      // Memory size between 1 and 20
        int arrival_time = rand() % 50;           // Arrival time between 0 and 49
        int total_cpu_time = (rand() % 100) + 10; // CPU time between 10 and 109
        int io_frequency = (rand() % 25) + 5;     // I/O frequency between 5 and 29
        int io_duration = (rand() % 10) + 1;      // I/O duration between 1 and 10
        int priority = (rand() % 5) + 1;          // Priority between 1 (high) and 5 (low)
        fprintf(file, "%d, %d, %d, %d, %d, %d, %d\n",
                pid, memory_size, arrival_time, total_cpu_time, io_frequency, io_duration, priority);
    }

    fclose(file);
    printf("Priority input data written to %s successfully.\n", filename);
}

ready_queue_t *alloc(void)
{
    ready_queue_t *queue = (ready_queue_t *)malloc(sizeof(ready_queue_t));
    assert(queue != NULL);
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

waiting_queue_t *alloc_state(void)
{
    waiting_queue_t *queue = (waiting_queue_t *)malloc(sizeof(waiting_queue_t));
    assert(queue != NULL);
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(ready_queue_t *queue, int process)
{
    assert(queue != NULL);
    node_t *node = (node_t *)malloc(sizeof(node_t));
    assert(node != NULL);
    node->process = process;
    node->next = NULL;
    if (queue->front == NULL)
    {
        queue->front = node;
        queue->rear = node;
    }
    else
    {
        queue->rear->next = node;
        queue->rear = node;
    }

    queue->size++;
}

void dequeue(ready_queue_t *queue)
{
    assert(queue != NULL);

    if (queue->front == NULL)
    {
        return;
    }
    else
    {
        node_t *to_remove = queue->front;
        if (queue->front->next == NULL)
        {
            queue->front = NULL;
            queue->rear = NULL;
        }
        else
        {
            queue->front = to_remove->next;
        }
        free(to_remove);
        queue->size--;
    }
}

int getArrivalTime(ready_queue_t *queue, int count)
{
    // Sort processes based on arrival time (ascending order)
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (pcb_table[i].arrival_time > pcb_table[j].arrival_time)
            {
                // Swap the processes
                pcb_t temp = pcb_table[i];
                pcb_table[i] = pcb_table[j];
                pcb_table[j] = temp;
            }
        }
    }

    // Enqueue the processes in sorted order
    for (int i = 0; i < count; i++)
    {
        enqueue(queue, pcb_table[i].pid);
    }

    return 0;
}

int getPriorityArrivalTime(ready_queue_t *queue, int count)
{
    // Sort processes based on arrival time and priority (ascending order)
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            // Sort by arrival time first
            if (pcb_table[i].arrival_time > pcb_table[j].arrival_time ||
                (pcb_table[i].arrival_time == pcb_table[j].arrival_time && pcb_table[i].priority > pcb_table[j].priority))
            {
                // Swap the processes
                pcb_t temp = pcb_table[i];
                pcb_table[i] = pcb_table[j];
                pcb_table[j] = temp;
            }
        }
    }

    // Enqueue the processes in sorted order
    for (int i = 0; i < count; i++)
    {
        enqueue(queue, pcb_table[i].pid);
    }

    return 0;
}

void save_state(waiting_queue_t *queue, int data, int process)
{
    assert(queue != NULL);
    link_t *node = (link_t *)malloc(sizeof(link_t));
    assert(node != NULL);
    node->data = data;
    node->process = process;
    node->next = NULL;
    if (queue->front == NULL)
    {
        queue->front = node;
        queue->rear = node;
    }
    else
    {
        queue->rear->next = node;
        queue->rear = node;
    }
    queue->size++;
}

int remove_state(waiting_queue_t *queue)
{
    assert(queue != NULL);

    if (queue->front == NULL)
    {
        return -1;
    }
    else
    {
        link_t *to_remove = queue->front;
        if (queue->front->next == NULL)
        {
            queue->front = NULL;
            queue->rear = NULL;
        }
        else
        {
            queue->front = to_remove->next;
        }
        free(to_remove);
        queue->size--;
    }
    return 0;
}

int find_pcb(int pid, int counter)
{
    for (int i = 0; i < counter; i++)
    {
        if (pcb_table[i].pid == pid)
        {
            return i;
        }
    }
    return -1; // PID not found
}

void sort_ready_queue(ready_queue_t *queue, int counter)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty, nothing to sort.\n");
        return;
    }

    // Pointer to the current node being sorted
    node_t *current = queue->front;

    // Selection sort based on `io_duration`
    while (current != NULL)
    {
        // Assume the current node contains the smallest value of `io_duration`
        node_t *min_node = current;
        node_t *next = current->next;

        // Find the smallest `arrival_time` in the remaining list
        while (next != NULL)
        {
            // Get the PCB index for the processes
            int current_pcb_index = find_pcb(min_node->process, counter);
            int next_pcb_index = find_pcb(next->process, counter);

            // Ensure the PCB indices are valid
            if (current_pcb_index == -1 || next_pcb_index == -1)
            {
                fprintf(stderr, "Error: PCB for PID %d or %d not found.\n",
                        min_node->process, next->process);
                next = next->next;
                continue;
            }

            // Compare `arrival_time` of the processes
            if (pcb_table[next_pcb_index].arrival_time < pcb_table[current_pcb_index].arrival_time)
            {
                min_node = next;
            }
            next = next->next;
        }

        // Swap the process IDs if needed
        if (min_node != current)
        {
            int temp = current->process;
            current->process = min_node->process;
            min_node->process = temp;
        }

        // Move to the next node
        current = current->next;
    }
}

void sort_priority_ready_queue(ready_queue_t *queue, int counter)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty, nothing to sort.\n");
        return;
    }

    // Pointer to the current node being sorted
    node_t *current = queue->front;

    // Selection sort based on `io_duration`
    while (current != NULL)
    {
        // Assume the current node contains the smallest value of `io_duration`
        node_t *min_node = current;
        node_t *next = current->next;

        // Find the smallest `arrival_time` in the remaining list
        while (next != NULL)
        {
            // Get the PCB index for the processes
            int current_pcb_index = find_pcb(min_node->process, counter);
            int next_pcb_index = find_pcb(next->process, counter);

            // Ensure the PCB indices are valid
            if (current_pcb_index == -1 || next_pcb_index == -1)
            {
                fprintf(stderr, "Error: PCB for PID %d or %d not found.\n",
                        min_node->process, next->process);
                next = next->next;
                continue;
            }

            // Compare `arrival_time` of the processes
            if (pcb_table[next_pcb_index].priority < pcb_table[current_pcb_index].priority)
            {
                min_node = next;
            }
            next = next->next;
        }

        // Swap the process IDs if needed
        if (min_node != current)
        {
            int temp = current->process;
            current->process = min_node->process;
            min_node->process = temp;
        }

        // Move to the next node
        current = current->next;
    }
}

void calculate_memory_status(partition_t partition[], int *memory_used, int *total_free_memory, int *usable_free_memory, char *partitions_state, int memory_size)
{
    int used_memory = 0;
    int free_memory = 0;
    int usable_memory = 0;

    // Prepare the partitions state string
    strcpy(partitions_state, "");
    for (int i = 0; i < NUM_PARTITIONS; i++)
    {
        if (partition[i].occupied != -1)
        {
            used_memory = memory_size; // Add to used memory
            char temp[10];
            sprintf(temp, "%d", partition[i].occupied);
            strcat(partitions_state, temp);
        }
        else
        {
            free_memory = 100 - memory_size;    // Add to free memory
            usable_memory += partition[i].size; // Usable memory
            strcat(partitions_state, "-1");
        }
        if (i < NUM_PARTITIONS - 1)
            strcat(partitions_state, ", ");
    }

    *memory_used = used_memory;
    *total_free_memory = free_memory;
    *usable_free_memory = usable_memory;
}

void log_memory_status(FILE *fptr, partition_t partition[], int time_of_event, int memory_size)
{
    int memory_used, total_free_memory, usable_free_memory;
    char partitions_state[100]; // Buffer for partition state

    // Calculate the memory status
    calculate_memory_status(partition, &memory_used, &total_free_memory, &usable_free_memory, partitions_state, memory_size);

    // Write to the file
    fprintf(fptr, "| %10d | %11d | %-30s | %15d | %15d |\n",
            time_of_event, memory_used, partitions_state, total_free_memory, usable_free_memory);
}

void free_memory(partition_t partition[], int partition_num)
{
    for (int i = 0; i < NUM_PARTITIONS; i++)
    {
        if (partition[i].occupied == partition_num)
        {
            partition[i].occupied = -1; // Mark as free
            return;
        }
    }
    fprintf(stderr, "Error: Partition %d not found.\n", partition_num);
}

void analyze_metrics(pcb_t pcb_table[], int process_count, int total_simulation_time)
{
    int completed_processes = 0;
    int total_turnaround_time = 0;
    int total_wait_time = 0;

    for (int i = 0; i < process_count; i++)
    {
        if (strcmp(pcb_table[i].current_state, "terminated") == 0)
        {
            completed_processes++;
            total_turnaround_time += pcb_table[i].turnaround_time;
            total_wait_time += pcb_table[i].total_wait_time;
        }
    }

    double throughput = (double)completed_processes / total_simulation_time;
    double average_turnaround_time = (double)total_turnaround_time / completed_processes;
    double average_wait_time = (double)total_wait_time / completed_processes;

    printf("Simulation Results:\n");
    printf("Throughput: %.2f processes/unit time\n", throughput);
    printf("Average Turnaround Time: %.2f units\n", average_turnaround_time);
    printf("Average Wait Time: %.2f units\n", average_wait_time);
}

void run_fcfs_scheduler(ready_queue_t *ready_queue, partition_t partition[], FILE *fptr, FILE *log_memory, int counter)
{
    int current_time = 0;                           // Simulated time
    waiting_queue_t *waiting_queue = alloc_state(); // Allocate waiting queue
    int io_completed = 0;
    int partition_size;
    int partition_num;
    int memory_size = 0;

    for (int i = 0; i < counter; i++)
    {
        if (memory_size == 0)
        {
            log_memory_status(log_memory, partition, 0, memory_size);
        }
        // Handle NEW -> READY transition
        if (strcmp(pcb_table[i].current_state, "new") == 0)
        {
            if (current_time < pcb_table[i].arrival_time)
            {
                break;
            }
            strcpy(pcb_table[i].next_state, "ready");
            // printf("%d\n", pcb_table[i].arrival_time);
            fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", pcb_table[i].arrival_time, pcb_table[i].pid, pcb_table[i].current_state, pcb_table[i].next_state);
            strcpy(pcb_table[i].current_state, "ready");
            memory_size += pcb_table[i].memory_size;
            partition_size = getpartition_size(partition, pcb_table[i].memory_size, pcb_table[i].pid);
            partition_num = getpartition_num(partition, partition_size);
            log_memory_status(log_memory, partition, pcb_table[i].arrival_time, memory_size);
        }
        if (ready_queue->front->process == pcb_table[i].pid)
        {
            current_time = pcb_table[i].arrival_time + pcb_table[i].io_frequency;
            strcpy(pcb_table[i].next_state, "running");
            fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", pcb_table[i].arrival_time, pcb_table[i].pid, pcb_table[i].current_state, pcb_table[i].next_state);
            strcpy(pcb_table[i].current_state, "running");
        }
    }

    while (ready_queue->size > 0 || waiting_queue->size > 0)
    {
        // Process from the ready queue
        if (ready_queue->front != NULL)
        {
            int pid = ready_queue->front->process;

            // Find PCB for the process
            int pcb_index = find_pcb(pid, counter);
            if (pcb_index == -1)
            {
                fprintf(stderr, "Error: PID %d not found in PCB table.\n", pid);
                dequeue(ready_queue);
                continue;
            }

            pcb_t *pcb = &pcb_table[pcb_index];
            if (strcmp(pcb->current_state, "new") == 0)
            {
                strcpy(pcb->next_state, "ready");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "ready");

                memory_size += pcb->memory_size;
                partition_size = getpartition_size(partition, pcb->memory_size, pcb->pid);
                partition_num = getpartition_num(partition, partition_size);
                log_memory_status(log_memory, partition, current_time, memory_size);
            }
            // Handle READY -> RUNNING transition
            else if (strcmp(pcb->current_state, "ready") == 0)
            {
                strcpy(pcb->next_state, "running");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "running");

                // Simulate a CPU burst
                int cpu_burst = pcb->io_frequency;
                current_time += cpu_burst;
                pcb->remaining_time -= cpu_burst;
                if (pcb->remaining_time < pcb->io_frequency)
                {
                    current_time += pcb->remaining_time;
                }

                pcb->total_burst_time += cpu_burst;

                // Check if the process is finished
                if (pcb->remaining_time <= 0)
                {
                    strcpy(pcb->next_state, "terminated");
                    fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                    strcpy(pcb->current_state, "terminated");

                    pcb->completion_time = current_time;
                    pcb->turnaround_time = pcb->completion_time - pcb->arrival_time;

                    memory_size -= pcb->memory_size;
                    partition_num = pcb->pid;
                    // Free memory partition and remove process
                    free_memory(partition, partition_num);
                    log_memory_status(log_memory, partition, current_time, memory_size);
                    dequeue(ready_queue);
                }
            }
            else if (strcmp(pcb->current_state, "running") == 0)
            {
                strcpy(pcb->next_state, "waiting");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "waiting");
                io_completed = 0;
                // Save process to the waiting queue
                save_state(waiting_queue, current_time + pcb->io_duration, pid);
                dequeue(ready_queue);
            }
        }
        // Process the waiting queue
        if (waiting_queue->front != NULL)
        {
            int completed_pid = waiting_queue->front->process;
            int pcb_index = find_pcb(completed_pid, counter);
            if (pcb_index == -1)
            {
                fprintf(stderr, "Error: PID %d not found in PCB table.\n", completed_pid);
                remove_state(waiting_queue);
                continue;
            }

            pcb_t *pcb = &pcb_table[pcb_index];
            int completion_time = waiting_queue->front->data;

            // printf("%d, %d, %d\n", completion_time, current_time, completed_pid);
            // Handle I/O completion
            if (completion_time <= current_time)
            {
                if (io_completed == 1)
                {
                    current_time = completion_time;
                }
                strcpy(pcb->next_state, "ready");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", completion_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "ready");

                pcb->total_wait_time += (current_time - pcb->arrival_time);
                io_completed = 1;

                // Requeue process in the ready queue and remove from waiting queue
                enqueue(ready_queue, completed_pid);
                sort_ready_queue(ready_queue, counter);
                remove_state(waiting_queue);
            }
            else
            {
                save_state(waiting_queue, completion_time, completed_pid);
                remove_state(waiting_queue);
            }
            // Advance time to the next waiting queue event if the ready queue is empty
            if (ready_queue->size == 0 && waiting_queue->front != NULL)
            {
                current_time = waiting_queue->front->data;
            }
        }
        if (ready_queue->size == 0 && waiting_queue->size == 0)
        {
            break; // End the simulation
        }
    }

    analyze_metrics(pcb_table, counter, current_time);

    fprintf(fptr, "+---------------------------------------------------+\n");
}

void run_priority_scheduler(ready_queue_t *ready_queue, partition_t partition[], FILE *fptr, FILE *log_memory, int counter)
{
    int current_time = 0;                           // Simulated time
    waiting_queue_t *waiting_queue = alloc_state(); // Allocate waiting queue
    int io_completed = 0;
    int partition_size;
    int partition_num;
    int memory_size = 0;
    
    log_memory_status(log_memory, partition, 0, memory_size);

    while (ready_queue->size > 0 || waiting_queue->size > 0)
    {
        // Process from the ready queue
        if (ready_queue->front != NULL)
        {
            int pid = ready_queue->front->process;

            // Find PCB for the process
            int pcb_index = find_pcb(pid, counter);
            if (pcb_index == -1)
            {
                fprintf(stderr, "Error: PID %d not found in PCB table.\n", pid);
                dequeue(ready_queue);
                continue;
            }

            pcb_t *pcb = &pcb_table[pcb_index];

            if (strcmp(pcb->current_state, "new") == 0)
            {
                strcpy(pcb->next_state, "ready");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "ready");

                memory_size += pcb->memory_size;
                partition_size = getpartition_size(partition, pcb->memory_size, pcb->pid);
                partition_num = getpartition_num(partition, partition_size);
                log_memory_status(log_memory, partition, current_time, memory_size);
            }
            // Handle READY -> RUNNING transition
            if (strcmp(pcb->current_state, "ready") == 0)
            {
                strcpy(pcb->next_state, "running");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "running");

                // Simulate a CPU burst
                int cpu_burst = pcb->io_frequency;
                current_time += cpu_burst;
                pcb->remaining_time -= cpu_burst;
                if (pcb->remaining_time < pcb->io_frequency)
                {
                    current_time += pcb->remaining_time;
                }

                pcb->total_burst_time += cpu_burst;

                // Check if the process is finished
                if (pcb->remaining_time <= 0)
                {
                    strcpy(pcb->next_state, "terminated");
                    fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                    strcpy(pcb->current_state, "terminated");

                    pcb->completion_time = current_time;
                    pcb->turnaround_time = pcb->completion_time - pcb->arrival_time;

                    memory_size -= pcb->memory_size;
                    partition_num = pcb->pid;
                    // printf("%d\n", pcb->pid);
                    // Free memory partition and remove process
                    free_memory(partition, partition_num);
                    log_memory_status(log_memory, partition, current_time, memory_size);
                    dequeue(ready_queue);
                }
            }
            else if (strcmp(pcb->current_state, "running") == 0)
            {
                strcpy(pcb->next_state, "waiting");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", current_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "waiting");
                io_completed = 0;
                // Save process to the waiting queue
                save_state(waiting_queue, current_time + pcb->io_duration, pid);
                dequeue(ready_queue);
            }
        }
        // Process the waiting queue
        if (waiting_queue->front != NULL)
        {
            int completed_pid = waiting_queue->front->process;
            int pcb_index = find_pcb(completed_pid, counter);
            if (pcb_index == -1)
            {
                fprintf(stderr, "Error: PID %d not found in PCB table.\n", completed_pid);
                remove_state(waiting_queue);
                continue;
            }

            pcb_t *pcb = &pcb_table[pcb_index];
            int completion_time = waiting_queue->front->data;

            // printf("%d, %d, %d\n", completion_time, current_time, completed_pid);
            // Handle I/O completion
            if (completion_time <= current_time)
            {
                if (io_completed == 1)
                {
                    current_time = completion_time;
                }
                strcpy(pcb->next_state, "ready");
                fprintf(fptr, "| %18d | %-4d | %9s | %9s |\n", completion_time, pcb->pid, pcb->current_state, pcb->next_state);
                strcpy(pcb->current_state, "ready");

                pcb->total_wait_time += (current_time - pcb->arrival_time);
                io_completed = 1;

                // Requeue process in the ready queue and remove from waiting queue
                enqueue(ready_queue, completed_pid);
                sort_priority_ready_queue(ready_queue, counter);
                remove_state(waiting_queue);
            }
            else
            {
                save_state(waiting_queue, completion_time, completed_pid);
                remove_state(waiting_queue);
            }
            // Advance time to the next waiting queue event if the ready queue is empty
            if (ready_queue->size == 0 && waiting_queue->front != NULL)
            {
                current_time = waiting_queue->front->data;
            }
        }
        if (ready_queue->size == 0 && waiting_queue->size == 0)
        {
            break; // End the simulation
        }
    }

    analyze_metrics(pcb_table, counter, current_time);

    fprintf(fptr, "+---------------------------------------------------+\n");
}

int readProgram(ready_queue_t *ready_queue, partition_t partition[], const char *filename_r, FILE *fptr, FILE *log_memory, int priority_scheduler)
{
    FILE *file = fopen(filename_r, "r");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return -1;
    }

    fprintf(fptr, "+---------------------------------------------------+\n");
    fprintf(fptr, "| Time of Transition | PID  | Old State | New State |\n");
    fprintf(fptr, "+---------------------------------------------------+\n");

    fprintf(log_memory, "+--------------+-------------+--------------------------------+-------------------+-------------------+\n");
    fprintf(log_memory, "| Time of Event | Memory Used |       Partitions State         | Total Free Memory | Usable Free Memory |\n");
    fprintf(log_memory, "+--------------+-------------+--------------------------------+-------------------+-------------------+\n");

    int pid, memory_size, arrival_time, total_cpu_time, io_freq, io_duration, priority;
    int counter = 0;
    // Read the input file line by line
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (sscanf(buffer, "%d, %d, %d, %d, %d, %d, %d", &pid, &memory_size, &arrival_time, &total_cpu_time, &io_freq, &io_duration, &priority) == 7)
        {
            // Initialize PCB with priority
            initpcb(partition, pid, total_cpu_time, io_freq, io_duration, counter, partition[5].num);
            pcb_table[counter].memory_size = memory_size;
            pcb_table[counter].arrival_time = arrival_time;
            pcb_table[counter].priority = priority;
        }
        else
        {
            printf("Error reading line: %s\n", buffer);
            break;
        }
        counter++;
    }

    fclose(file);

    // Sort and enqueue processes based on the scheduler type
    if (priority_scheduler == 1)
    {
        getPriorityArrivalTime(ready_queue, counter);
        printf("Priority Queue Front: %d, Next: %d\n", ready_queue->front->process, pcb_table[1].pid);
        // Run the FCFS scheduler
        run_priority_scheduler(ready_queue, partition, fptr, log_memory, counter);
    }
    else
    {
        getArrivalTime(ready_queue, counter);
        // Run the FCFS scheduler
        run_fcfs_scheduler(ready_queue, partition, fptr, log_memory, counter);
    }

    return 0;
}

// Comparison function for qsort
int compare_log_entries(const void *a, const void *b)
{
    log_entry_t *entry1 = (log_entry_t *)a;
    log_entry_t *entry2 = (log_entry_t *)b;

    // Compare based on time_of_transition
    if (entry1->time_of_transition != entry2->time_of_transition)
    {
        return entry1->time_of_transition - entry2->time_of_transition;
    }

    // If times are equal, compare based on PID
    return entry1->pid - entry2->pid;
}

// Function to sort the log and overwrite the file
void sort_log_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    log_entry_t log_entries[1000]; // Assuming a maximum of 1000 log entries
    int count = 0;

    // Skip the first three lines (headers)
    char buffer[256];
    fgets(buffer, sizeof(buffer), file); // Skip "+---------------------------------------------------+"
    fgets(buffer, sizeof(buffer), file); // Skip "| Time of Transition | PID  | Old State | New State |"
    fgets(buffer, sizeof(buffer), file); // Skip "+---------------------------------------------------+"

    int time, pid;
    char current[20];
    char next[20];

    // Use fscanf to read each line into the log_entry_t structure
    while (fscanf(file, "| %d | %d | %s | %s |\n", &time, &pid, current, next) == 4)
    {
        log_entries[count].time_of_transition = time;
        log_entries[count].pid = pid;
        strcpy(log_entries[count].old_state, current);
        strcpy(log_entries[count].new_state, next);
        count++;
    }

    printf("Number of log entries read: %d\n", count);

    fclose(file);

    // Sort the log entries
    qsort(log_entries, count, sizeof(log_entry_t), compare_log_entries);

    // Overwrite the file with the sorted log
    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    fprintf(file, "+---------------------------------------------------+\n");
    fprintf(file, "| Time of Transition | PID  | Old State | New State |\n");
    fprintf(file, "+---------------------------------------------------+\n");
    for (int i = 0; i < count; i++)
    {
        fprintf(file, "| %18d | %-4d | %9s | %9s |\n",
                log_entries[i].time_of_transition,
                log_entries[i].pid,
                log_entries[i].old_state,
                log_entries[i].new_state);
    }
    fprintf(file, "+---------------------------------------------------+\n");

    fclose(file);
}

int main(int argc, char *argv[])
{
    srand(time(0));

    // Generate input data files
    // generate_input_data("input_data_Agrim_Kasaju_Chukwuemeka_Igwe.txt");
    // generate_priority_input_data("input_data_2_Agrim_Kasaju_Chukwuemeka_Igwe.txt");
    // Generate non-priority files
    // for (int i = 0; i < 5; i++) {
    //     generate_non_priority_file(i + 1);
    // }

    // Generate priority files
    // for (int i = 0; i < 10; i++) {
    //     generate_priority_file(i + 1);
    // }

    // Validate input arguments
    if (argc < 3)
    {
        printf("Usage: %s <input trace filename> <output log filename>\n", argv[0]);
        return 1;
    }

    // Initialize partitions
    partition_t partition[6];
    initpartition(partition);

    // Allocate memory for the ready queue
    ready_queue_t *queue = alloc();

    // Open output files
    const char *input_file = argv[1];
    char *scheduler = argv[2];

    const char *output_file = "execution_101280101_101219454.txt";
    FILE *fptr = fopen(output_file, "w");
    FILE *log_memory = fopen("memory_status_101280101_101219454.txt", "w");

    if (fptr == NULL || log_memory == NULL)
    {
        printf("Error opening one of the output files.\n");
        return 1;
    }

    int priority_scheduler = 0;

    if (strcmp(scheduler, "EP") == 0) {
        priority_scheduler = 1;
    } else if (strcmp(scheduler, "FCFS") == 0) {
        priority_scheduler = 0;
    } else {
        printf("Error: Unsupported scheduler type. Use 'FCFS' or 'EP'.\n");
        return 1;
    }

    // Read the program and process the input file
    readProgram(queue, partition, input_file, fptr, log_memory, priority_scheduler);

    // Close output files
    fclose(fptr);
    fclose(log_memory);

    // Sort the generated log file
    sort_log_file(output_file);

    return 0;
}
