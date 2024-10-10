#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>   
#include <sys/wait.h>    
#include <fcntl.h>       

#define BUF_SZ 5 // Buffer size for the pipe
#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

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

    int pipe_fd[2]; // Pipe file descriptors
    if (pipe(pipe_fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        srand(time(NULL)); // Seed for random delay
        long long a = 0; // First Fibonacci number
        long long b = 1; // Second Fibonacci number

        // Close the read end of the pipe in the child process
        close(pipe_fd[PIPE_READ_END]);

        for (int i = 0; i < n; i++) {
            // Simulate random delay
            int delay = rand() % 3;
            sleep(delay);

            // Send the next Fibonacci number to the pipe
            long long next_fib = a; // Current Fibonacci number to send
            write(pipe_fd[PIPE_WRITE_END], &next_fib, sizeof(next_fib)); // Write to the pipe

            // Update Fibonacci numbers
            long long temp = a;
            a = b;
            b = temp + b;

            printf("[Child] Produced: %lld (delay %d seconds)\n", next_fib, delay);
            fflush(stdout); // Ensure output is flushed immediately
        }

        // Close the write end of the pipe after sending all numbers
        close(pipe_fd[PIPE_WRITE_END]);
        exit(EXIT_SUCCESS);
    } else { // Parent process
        long long num;
        // Close the write end of the pipe in the parent process
        close(pipe_fd[PIPE_WRITE_END]);

        for (int i = 0; i < n; i++) {
            // Read the Fibonacci number from the pipe
            read(pipe_fd[PIPE_READ_END], &num, sizeof(num));

            printf("[Parent] Consumed: %lld\n", num);
            fflush(stdout); // Ensure output is flushed immediately
        }

        // Close the read end of the pipe after consuming all numbers
        close(pipe_fd[PIPE_READ_END]);

        // Wait for the child process to finish
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
