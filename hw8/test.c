#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define DEVICE_PATH "/dev/lab8"

int main(int argc, char *argv[]) {
    int fd;
    char *buffer;
    ssize_t bytes_read;

    // Check if buffer size is provided as a command-line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <BUFFER_SIZE>\n", argv[0]);
        return 1;
    }

    // Parse the buffer size from the command-line argument
    int buffer_size = atoi(argv[1]);
    if (buffer_size <= 0) {
        fprintf(stderr, "Invalid BUFFER_SIZE: %d\n", buffer_size);
        return 1;
    }

    // Dynamically allocate memory for the buffer
    buffer = malloc(buffer_size);
    if (!buffer) {
        perror("Failed to allocate memory for buffer");
        return 1;
    }

    // Open the device
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        free(buffer); // Free allocated memory
        return 1;
    }
    printf("Successfully opened device: %s\n", DEVICE_PATH);

    // Read from the device
    bytes_read = read(fd, buffer, buffer_size); // buffer_size -1, Leave space for null terminator?
    if (bytes_read < 0) {
        perror("Failed to read from device");
        close(fd);
        free(buffer); // Free allocated memory
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
    free(buffer); // Free allocated memory
    printf("Closed device\n");

    return 0;
}
