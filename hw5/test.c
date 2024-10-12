#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// verify that the child process can modify the value of 'n' and the parent process can still access the original value of 'n'

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    // Parse the command-line argument
    int n = atoi(argv[1]);

    // Fork the process
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child process: n = %d\n", n);
        // Child can access 'n' directly as it's inherited from the parent
        n *= 2;  // Modify 'n' in the child
        printf("Child process: modified n = %d\n", n);
    } else {
        // Parent process
        printf("Parent process: n = %d\n", n);
        // Wait for the child to finish
        wait(NULL);
    }

    return 0;
}
