#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define NUM_TA 5
#define NUM 20
#define MARKING_ROUNDS 3
#define STUDENT_FILE "student_database.txt"

struct semaphore
{
    sem_t sem;      // Semaphore for synchronizing TAs
};

void semaphore_init(struct semaphore *s, int value)
{
    sem_init(&s->sem, 0, value);
}

// Wait (P operation) with condition
int P(struct semaphore *s)
{
    // Lock the semaphore
    return sem_wait(&s->sem);
}

// Signal (V operation) with condition
int V(struct semaphore *s)
{
    // Unlock the semaphore
    return sem_post(&s->sem);
}

void create_student_database(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error creating student database file");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for (int i = 0; i < NUM - 1; i++)
    {
        fprintf(file, "%04d\n", rand() % 9999 + 1);
    }
    fprintf(file, "9999\n");
    fclose(file);
}

void mark_exams(struct semaphore *semaphores, int id, int *student_numbers, int *shared_index)
{
    int current = id;
    int next = (id + 1) % NUM_TA;

    char filename[20];
    sprintf(filename, "TA%d.txt", id + 1);

    FILE *fptr = fopen(filename, "w");
    if (fptr == NULL)
    {
        perror("Error opening TA file");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) + getpid());

    int rounds_completed = 0;

    while (rounds_completed < MARKING_ROUNDS)
    {
        printf("TA %d is waiting to access the database.\n", id + 1);
        sleep(rand() % 4 + 1); // Simulate waiting delay

        // Acquire individual semaphores
        P(&semaphores[current]);
        if (P(&semaphores[next]) != 0)
        {
            // Release both if the next semaphore is unavailable
            V(&semaphores[current]);
            printf("TA %d realeased semaphores %d and %d.\n", id + 1, current, next);
            continue;
        }
        printf("TA %d acquired semaphores %d and %d.\n", id + 1, current, next);

        // Mark student
        int student_number = student_numbers[*shared_index];
        if (student_number == 9999)
        {
            rounds_completed++;
            *shared_index = 0;
            printf("TA %d completed round %d.\n", id + 1, rounds_completed);
        }
        else
        {
            // Release semaphores
            V(&semaphores[current]);
            V(&semaphores[next]);
            printf("TA %d realeased semaphores %d and %d.\n", id + 1, current, next);
            
            (*shared_index)++;
            int mark = rand() % 10 + 1;
            fprintf(fptr, "Student: %04d, Mark: %d\n", student_number, mark);
            printf("TA %d is marking student %04d with mark %d.\n", id + 1, student_number, mark);
            sleep(rand() % 4 + 1); // Simulate marking delay
        }
    }

    fclose(fptr);
}

int main()
{
    create_student_database(STUDENT_FILE);

    int shmid = shmget(IPC_PRIVATE, (NUM + 1) * sizeof(int), IPC_CREAT | 0666);
    int *shared_data = (int *)shmat(shmid, NULL, 0);
    int *student_numbers = shared_data;
    int *shared_index = shared_data + NUM;
    *shared_index = 0;

    FILE *file = fopen(STUDENT_FILE, "r");
    if (file == NULL)
    {
        perror("Error opening student database file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM; i++)
    {
        fscanf(file, "%d", &student_numbers[i]);
    }
    fclose(file);

    struct semaphore semaphores[NUM_TA];
    for (int i = 0; i < NUM_TA; i++)
    {
        semaphore_init(&semaphores[i], 1);
    }

    pid_t pids[NUM_TA];
    for (int i = 0; i < NUM_TA; i++)
    {
        pids[i] = fork();
        if (pids[i] == 0)
        {
            mark_exams(semaphores, i, student_numbers, shared_index);
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
    shmdt(shared_data);
    shmctl(shmid, IPC_RMID, NULL);

    printf("All TAs have finished marking exams.\n");
    return 0;
}
