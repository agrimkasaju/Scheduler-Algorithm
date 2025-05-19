#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define NUM_TA 5
#define NUM 20
#define STUDENT_FILE "student_database.txt"

struct semaphore {
    sem_t sem;
};

// Initialize semaphore
void semaphore_init(struct semaphore *s, int value)
{
    sem_init(&s->sem, 0, value); // Initialize semaphore with given value
}

// Wait (P operation)
void P(struct semaphore *s)
{
    sem_wait(&s->sem); // Wait on semaphore
}

// Signal (V operation)
void V(struct semaphore *s)
{
    sem_post(&s->sem); // Signal semaphore
}

void create_student_database(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error creating student database file");
        exit(EXIT_FAILURE);
    }

    // Generate random student numbers
    srand(time(NULL));
    for (int i = 0; i < NUM - 1; i++)
    {
        fprintf(file, "%04d ", rand() % 9999 + 1); // Generate numbers from 0001 to 9999
    }
    fprintf(file, "9999\n"); // End with 9999
    fclose(file);
}

void mark_exams(struct semaphore *semaphores, int id, int *student_num)
{
    int current = id;
    int next = (id + 1) % NUM_TA;

    char filename[20];
    sprintf(filename, "TA%d.txt", id + 1);

    FILE *fptr = fopen(filename, "w");
    if (fptr == NULL)
    {
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) + getpid());
    
    int count = 0;
    int i = 0;
    while (count < 3)
    {
        printf("TA %d is waiting to access the database.\n", id + 1);

        P(&semaphores[current]); // Lock TA's own semaphore
        P(&semaphores[next]); // Lock next TA's own semaphore

        // Simulate accessing the database and marking an exam
        int student_number = student_num[i];

        if (student_number == 9999)
        {
            count++;
            i = 0;
            printf("TA %d completed round %d.\n", id + 1, count);
        }
        else
        {
            int mark = rand() % 10 + 1;
            fprintf(fptr, "Student: %04d, Mark: %d\n", student_number, mark);
            printf("TA %d is marking student %04d with mark %d.\n", id + 1, student_number, mark);

            int marking_process = rand() % 4 + 1;
            sleep(marking_process);

            i++;
        }
        int wait_time = rand() % 4 + 1;
        sleep(wait_time);

        // Release the semaphores
        V(&semaphores[current]);
        V(&semaphores[next]); // Lock next TA's own semaphore
    }
    fclose(fptr);
}

int main()
{
    create_student_database(STUDENT_FILE);

    int student_number[NUM];
    FILE *file = fopen(STUDENT_FILE, "r");
    if (file == NULL)
    {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM; i++)
    {
        fscanf(file, "%d", &student_number[i]);
    }
    fclose(file);

    struct semaphore semaphores[NUM_TA];
    for (int i = 0; i < NUM_TA; i++)
    {
        semaphore_init(&semaphores[i], 1); // initialize semaphore with value 1
    }

    pid_t pids[NUM_TA];

    for (int i = 0; i < NUM_TA; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            mark_exams(semaphores, i, student_number);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_TA; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    for (int i = 0; i < NUM_TA; i++)
    {
        sem_destroy(&semaphores[i].sem);
    }

    printf("All TAs have finished marking exams.\n");
}