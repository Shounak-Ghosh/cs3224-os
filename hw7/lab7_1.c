#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define NUM_TRIALS 100000

// use gcc -o lab7_1 lab7_1.c -pthread to compile
// ./lab7_1 to run

pthread_mutex_t mutex;
int nhits = 0; // Shared variable to count successful trials

// param is a pointer to an int representing the number of students in the class
// void* is a generic pointer type in C
void* WorkerThread(void* param) {
    int n = *(int*)param;
    int local_hits = 0;
    int birthdays[n];
    
    // Seed the random number generator uniquely for each thread
    unsigned int rand_state = (unsigned int)time(NULL) + pthread_self();
    
    for (int i = 0; i < NUM_TRIALS; ++i) {
        for (int j = 0; j < n; ++j) {
            // generate birthdays
            birthdays[j] = rand_r(&rand_state) % 365;
        }
        // check collisions
        int found[365] = {0}; // Array to track birthdays
        for (int i = 0; i < n; ++i) {
            if (found[birthdays[i]]) {
                ++local_hits; // Found a duplicate
                break;
            }
            found[birthdays[i]] = 1;
        }
    }

    // Update global nhits with local_hits in a thread-safe manner
    pthread_mutex_lock(&mutex);
    nhits += local_hits;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s n1 n2 ... nn\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for (int k = 1; k < argc; ++k) {
        nhits = 0; // Reset nhits for each class size
        int n = atoi(argv[k]); // convert argument to int

        // Create and start threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, WorkerThread, &n);
        }

        // Wait for all threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        double probability = (double)nhits / (NUM_THREADS * NUM_TRIALS);
        printf("Probability of at least two students sharing a birthday in a class of %d students: %.2f%%.\n", n, probability * 100);
    }

    pthread_mutex_destroy(&mutex);
}