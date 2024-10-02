#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2, pid3, pid4;

    // Fork the first child (Child 1)
    pid1 = fork();

    if (pid1 == 0) {
        // Inside Child 1
        printf("Child 1 [PID: %d, Parent PID: %d]\n", getpid(), getppid());

        // Fork the third child (Child 3)
        pid3 = fork();
        if (pid3 == 0) {
            // Inside Child 3
            printf("Child 3 [PID: %d, Parent PID: %d]\n", getpid(), getppid());
        } else {
            // Wait for Child 3 to finish
            wait(NULL);
        }
    } else {
        // Fork the second child (Child 2)
        pid2 = fork();

        if (pid2 == 0) {
            // Inside Child 2
            printf("Child 2 [PID: %d, Parent PID: %d]\n", getpid(), getppid());

            // Fork the fourth child (Child 4)
            pid4 = fork();
            if (pid4 == 0) {
                // Inside Child 4
                printf("Child 4 [PID: %d, Parent PID: %d]\n", getpid(), getppid());
            } else {
                // Wait for Child 4 to finish
                wait(NULL);
            }
        } else {
            // Inside Parent
            printf("Parent [PID: %d]\n", getpid());

            // Wait for Child 1 and Child 2 to finish
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
