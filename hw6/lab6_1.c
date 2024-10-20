#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>

#define PORT 8080        // Define the port number for the server
#define MAX_ATTEMPTS 100 // Maximum number of connection attempts
#define BUF_SIZE 256     // Buffer size for socket communication

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    if (n <= 1) {
        printf("n must be greater than 1\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // Seed for random delay

    // Create a socket
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    pid_t pid;

    // Create the server socket (for the child)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the server to the specified port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Fork to create the child process
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process (server)
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd;
        long long a = 0, b = 1, next;

        // Random wait before starting the server (1 to 3 seconds)
        int delay = (rand() % 3) + 1;
        sleep(delay);

        // Start listening for connections
        listen(server_fd, 1);

        // Accept connection from the client
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Generate Fibonacci sequence and send it to the parent
        // n copied from forking parent process
        for (int i = 0; i < n; i++) {
            if (i == 0)
                next = a;
            else if (i == 1)
                next = b;
            else {
                next = a + b;
                a = b;
                b = next;
            }

            // Random delay (0 to 2 seconds) before sending each Fibonacci number
            int fib_delay = rand() % 3;
            sleep(fib_delay);

            // Send the next Fibonacci number to the parent
            write(client_fd, &next, sizeof(next));
        }

        // Close the client, server connections and exit process
        close(client_fd);
        close(server_fd);
        exit(EXIT_SUCCESS);
    }
    else { // Parent process (client)
        // Create a client socket
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        // Setup client address (same as server)
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(PORT);
        // use loopback as the client address
        client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // Try to connect repeatedly to the server
        int attempt = 0;
        while (connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
            attempt++;
            if (attempt >= MAX_ATTEMPTS) {
                perror("connect");
                exit(EXIT_FAILURE);
            }
            usleep(100000); // Wait 100 ms before the next attempt
        }

        // Send the value of n and print the Fibonacci sequence received from the child
        long long value;

        // Send the value of n to the child
        write(client_fd, &n, sizeof(n));

        // Receive Fibonacci numbers from the child and print them
        for (int i = 0; i < n; i++) {
            read(client_fd, &value, sizeof(value));
            printf("%lld ", value);
            fflush(stdout);
        }
        printf("\n");

        // Close the client socket
        close(client_fd);

        // Wait for the child to finish
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
