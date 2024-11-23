// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>

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

void generate_fibonacci(long long *fib_seq, long long length) {
    fib_seq[0] = 0;
    if (length > 1) fib_seq[1] = 1;
    for (long long i = 2; i < length; i++) {
        fib_seq[i] = fib_seq[i - 1] + fib_seq[i - 2];
    }
}

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

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shared_data_t *shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_data->in = 0;
    shared_data->out = 0;

    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUF_SZ);
    sem_t *sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    long long *fib_seq = malloc(n * sizeof(long long));
    if (!fib_seq) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    generate_fibonacci(fib_seq, n);

    struct timeval tv;
    struct tm *timeinfo;
    char time_str[20]; // String to hold the formatted time

    for (long long i = 0; i < n; i++) {
        sleep(rand() % 3);
        sem_wait(sem_empty);
        sem_wait(sem_mutex);

        // Get the current time
        gettimeofday(&tv, NULL);
        timeinfo = localtime(&tv.tv_sec);

        // Format the time up to seconds
        strftime(time_str, sizeof(time_str), "%S", timeinfo);

        shared_data->buffer[shared_data->in] = fib_seq[i];
        printf("(%s.%06ld) Produced Fibonacci[%lld]: %lld\n", time_str, tv.tv_usec, i, fib_seq[i]);
        shared_data->in = (shared_data->in + 1) % BUF_SZ;

        sem_post(sem_mutex);
        sem_post(sem_full);
    }

    free(fib_seq);
    munmap(shared_data, sizeof(shared_data_t));
    close(shm_fd);

    sem_close(sem_empty);
    sem_close(sem_full);
    sem_close(sem_mutex);

    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    sem_unlink(SEM_MUTEX);

    shm_unlink(SHM_NAME);

    return 0;
}
