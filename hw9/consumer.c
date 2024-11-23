// consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>

#define BUF_SZ 5
#define SHM_NAME "/lab9_shm"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define SEM_MUTEX "/sem_mutex"

static long long n = 1; // Should be initialized to a non-zero value

typedef struct {
    long long buffer[BUF_SZ];
    int in;
    int out;
} shared_data_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = atoll(argv[1]);
    if (n <= 1) {
        fprintf(stderr, "Error: n must be greater than 1.\n");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    shared_data_t *shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_full = sem_open(SEM_FULL, 0);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    struct tm *timeinfo;
    char time_str[20]; // String to hold the formatted time

    for (long long i = 0; i < n; i++) {
        sem_wait(sem_full);
        sem_wait(sem_mutex);

        long long value = shared_data->buffer[shared_data->out];
        shared_data->out = (shared_data->out + 1) % BUF_SZ;

        // Get the current time
        gettimeofday(&tv, NULL);
        timeinfo = localtime(&tv.tv_sec);

        // Format the time up to seconds
        strftime(time_str, sizeof(time_str), "%S", timeinfo);

        sem_post(sem_mutex);
        sem_post(sem_empty);


        printf("(%s.%06ld) Consumed Fibonacci[%lld]: %lld\n",time_str, tv.tv_usec, i, value);
        fflush(stdout);
    }

    munmap(shared_data, sizeof(shared_data_t));
    close(shm_fd);

    sem_close(sem_empty);
    sem_close(sem_full);
    sem_close(sem_mutex);

    return 0;
}
