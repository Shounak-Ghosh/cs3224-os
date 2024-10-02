#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t x = -11, y = -22, z = -33;
    
    x = fork();
    
    if (x == 0) {
        // Inside child process after the first fork
        printf("Process 2: x = %d, pid = %d, ppid = %d\n", x, getpid(), getppid());
        y = fork();
        
        if (y == 0) {
            // Inside child process of y fork
            printf("Process 3: x = %d, y = %d, pid = %d, ppid = %d\n", x, y, getpid(), getppid());
        } else if (y > 0) {
            z = fork();
            if (z == 0) {
                // Inside child process of z fork
                printf("Process 4: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
            } else {
                // Parent process after y and z forks
                printf("Process 2 (again): x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
            }
        }
    } else if (x > 0) {
        // Inside the original parent process
        printf("Process 1: x = %d, pid = %d, ppid = %d\n", x, getpid(), getppid());
    }
    
    return 0;
}
