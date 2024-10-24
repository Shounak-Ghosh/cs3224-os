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

int hasDuplicateBirthday(int *birthdays, int n) {
    int found[365] = {0}; // Array to track birthdays
    for (int i = 0; i < n; ++i) {
        if (found[birthdays[i]]) {
            return 1; // Found a duplicate
        }
        found[birthdays[i]] = 1;
    }
    return 0;
}

void* WorkerThread(void* param) {
    int n = *(int*)param;
    int local_hits = 0;
    int birthdays[n];
    
    // Seed the random number generator uniquely for each thread
    unsigned int rand_state = (unsigned int)time(NULL) + (unsigned int)pthread_self();
    
    for (int i = 0; i < NUM_TRIALS; ++i) {
        for (int j = 0; j < n; ++j) {
            birthdays[j] = rand_r(&rand_state) % 365;
        }
        if (hasDuplicateBirthday(birthdays, n)) {
            ++local_hits;
        }
    }

    // Update global nhits with local_hits in a thread-safe manner
    pthread_mutex_lock(&mutex);
    nhits += local_hits;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    int n_values[] = {49,23,41,128}; // Test with different class sizes

    for (int k = 0; k < sizeof(n_values)/sizeof(n_values[0]); ++k) {
        nhits = 0; // Reset nhits for each class size
        int n = n_values[k];

        // Create and start threads
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_create(&threads[i], NULL, WorkerThread, &n);
        }

        // Wait for all threads to finish
        for (int i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }

        double probability = (double)nhits / (NUM_THREADS * NUM_TRIALS);
        printf("Probability of at least two students sharing a birthday in a class of %d students: %.2f%%.\n", n, 100 * probability);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}