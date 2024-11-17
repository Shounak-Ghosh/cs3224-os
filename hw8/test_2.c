#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    char buffer[100];
    int bytes_read = 0;
    int fd = open("/dev/lab8", O_RDONLY);
    bytes_read = read(fd, buffer, 4);
    printf("Output: %s (%i bytes)\n", buffer, bytes_read);
}