#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define DEVICE_PATH "/dev/lab8"
#define BUFFER_SIZE 256

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Open the device
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    printf("Successfully opened device: %s\n", DEVICE_PATH);

    // Read from the device
    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    // Null-terminate the buffer and print the result
    buffer[bytes_read] = '\0';
    printf("Read %zd bytes from device: %s\n", bytes_read, buffer);

    // Convert the read UID to integer
    int read_uid = atoi(buffer);

    // Get the real UID of the current process
    uid_t real_uid = getuid();
    printf("Real User ID: %d\n", real_uid);

    // Compare the UIDs
    if (read_uid == real_uid) {
        printf("User IDs match!\n");
    } else {
        printf("User IDs do not match. Read UID: %d, Real UID: %d\n", read_uid, real_uid);
    }

    // Close the device
    close(fd);
    printf("Closed device\n");

    return 0;
}