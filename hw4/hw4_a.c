#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t x = -11, y = -22, z = -33;

    printf("Initial before x fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
    
    x = fork();

    printf("Initial after x fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
    
    if (x == 0) {
        printf("Inside process x before y fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
        y = fork();
        printf("Inside process x after y fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
    }
    printf("outside process x: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
    if (y > 0) {
        printf("outside process y before z fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
        z = fork();
        printf("outside process y after z fork: x = %d, y = %d, z = %d, pid = %d, ppid = %d\n", x, y, z, getpid(), getppid());
    }
    
    return 0;
}
