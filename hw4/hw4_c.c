#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    unsigned long long n = atoll(argv[1]);

    if (n <= 2) {
        fprintf(stderr, "Please provide n > 2\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Error in fork
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process: Print the first n Fibonacci numbers
        printf("Child process (PID: %d): Fibonacci sequence of length %llu:\n", getpid(), n);
        unsigned long long a = 0, b = 1;
        unsigned long long next;

        // Print the first n Fibonacci numbers
        for (unsigned long long i = 0; i < n; ++i) {
            if (i == 0) {
                printf("%llu ", a);
            } else if (i == 1) {
                printf("%llu ", b);
            } else {
                next = a + b;
                printf("%llu ", next);
                a = b;
                b = next;
            }
        }
        printf("\n");
        exit(0);
    } else {
        // Parent process: Wait for child to finish
        wait(NULL);
        printf("Parent process (PID: %d): Adding 2 more Fibonacci numbers:\n", getpid());

        // Calculate and print two more Fibonacci numbers
        unsigned long long a = 0, b = 1, next = 0;
        for (unsigned long long i = 0; i < n; ++i) {
            next = a + b;
            a = b;
            b = next;
        }
        printf("%llu %llu\n", a, b);
    }

    return 0;
}
