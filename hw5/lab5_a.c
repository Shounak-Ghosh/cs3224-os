#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define BUF_SZ 5 // Buffer size
#define SHM_NAME "/fibonacci_shm" // Name for the shared memory object

// Shared memory structure
typedef struct {
    long long buffer[BUF_SZ]; // Buffer to store Fibonacci numbers
    int in;                    // Index for producer (child process)
    int out;                   // Index for consumer (parent process)
    int count;                 // Count of elements in the buffer
} shared_mem_t;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 1) {
        fprintf(stderr, "n must be greater than 1\n");
        exit(EXIT_FAILURE);
    }

    // Create shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Configure the size of the shared memory
    if (ftruncate(shm_fd, sizeof(shared_mem_t)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object
    shared_mem_t* shared_mem = mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    shared_mem->in = 0;
    shared_mem->out = 0;
    shared_mem->count = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        srand(time(NULL)); // Seed for random delay
        long long a = 0; // First Fibonacci number
        long long b = 1; // Second Fibonacci number

        // when forking, memory is copied, so n is a local copy from the parent process
        for (int i = 0; i < n; i++) {
            // Simulate random delay
            int delay = rand() % 3;
            sleep(delay);

            // Wait until there's space in the buffer
            while (shared_mem->count == BUF_SZ) {
                usleep(100000); // Sleep for 100 milliseconds to reduce CPU usage
            }

            // Send the next Fibonacci number to the buffer
            long long next_fib = a; // Current Fibonacci number to send
            shared_mem->buffer[shared_mem->in] = next_fib; // Store in shared buffer
            shared_mem->in = (shared_mem->in + 1) % BUF_SZ;
            shared_mem->count++;

            // Update Fibonacci numbers
            long long temp = a;
            a = b;
            b = temp + b;
        }

        // Detach from shared memory
        munmap(shared_mem, sizeof(shared_mem_t));
        close(shm_fd);
        shm_unlink(SHM_NAME); // Unlink the shared memory

        exit(EXIT_SUCCESS);
    } else { // Parent process
        for (int i = 0; i < n; i++) {
            // Wait until there's data in the buffer
            while (shared_mem->count == 0) {
                usleep(100000); // Sleep for 100 milliseconds to reduce CPU usage
            }

            // Consume the Fibonacci number from the buffer
            long long num = shared_mem->buffer[shared_mem->out];
            shared_mem->out = (shared_mem->out + 1) % BUF_SZ;
            shared_mem->count--;

            printf("%lld ", num);
            fflush(stdout); // Ensure output is flushed immediately
        }
        printf("\n");

        // Wait for the child process to finish
        wait(NULL);

        // Detach from shared memory
        munmap(shared_mem, sizeof(shared_mem_t));
        close(shm_fd);
        shm_unlink(SHM_NAME); // Unlink the shared memory

        exit(EXIT_SUCCESS);
    }

    return 0;
}
