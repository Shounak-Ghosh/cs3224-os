#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For getuid()
#include <fcntl.h>   // For open() and O_* flags
#include <sys/types.h> // For ssize_t
#include <sys/stat.h>  // For file permissions
#include <string.h>   // For strlen()

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        // Check for exactly two arguments: input file and output file
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Hardcoded user name
    const char *user_name = "Shounak Ghosh";
    
    // Get user ID
    uid_t user_id = getuid();
    
    // Open the input file for reading
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Open the output file for writing (create if it doesn't exist, clear if it does)
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR);
    if (output_fd < 0) {
        perror("Error opening/creating output file");
        close(input_fd);
        exit(EXIT_FAILURE);
    }

    // Write user information to the output file
    char user_info[100];
    sprintf(user_info, "%s\n%d\n", user_name, user_id);
    if (write(output_fd, user_info, strlen(user_info)) != strlen(user_info)) {
        perror("Error writing user information to output file");
        close(input_fd);
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    // Copy the content from input file to output file
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) {
        if (write(output_fd, buffer, bytes_read) != bytes_read) {
            perror("Error writing to output file");
            close(input_fd);
            close(output_fd);
            exit(EXIT_FAILURE);
        }
    }
    
    if (bytes_read < 0) {
        perror("Error reading input file");
        close(input_fd);
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    // Close files
    close(input_fd);
    close(output_fd);

    return 0;
}
