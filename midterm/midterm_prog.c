#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <time.h>

#define NUM_TRIALS 1000000  
#define NUM_PROCS 4         

bool has_dup(int *birthdays, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (birthdays[i] == birthdays[j]) {
                return true;
            }
        }
    }
    return false;
}

void simulate(int n, sem_t *semaphore, int *nhits) {
    int local_hits = 0;

    for (int i = 0; i < NUM_TRIALS; i++) {
        int *birthdays = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            birthdays[j] = rand() % 365;  
        }

        if (has_dup(birthdays, n)) {
            local_hits++;
        }

        free(birthdays);
    }

    sem_wait(semaphore);  
    *nhits += local_hits; 
    sem_post(semaphore);  
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n> \n", argv[0]);
        return 1;
    }


    int n = atoi(argv[1]);

    int *nhits = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *nhits = 0; 

    sem_t *semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(semaphore, 1, 1); 


    pid_t pid1, pid2, pid3, pid4;

    pid1 = fork();

    if (pid1 == 0) {
        srand(time(NULL) + getpid());
        simulate(n,semaphore,nhits);
        exit(0);
    }

    pid2 = fork();

    if (pid2 == 0) {
        pid3 = fork();
        if (pid3 == 0) {
            pid4 = fork();
            if(pid4 == 0) {
                srand(time(NULL) + getpid());
                simulate(n,semaphore,nhits);
                exit(0);
            }
            else {
                wait(NULL);
            }
            srand(time(NULL) + getpid());
            simulate(n,semaphore,nhits);
            exit(0);
        }
        else {
            wait(NULL);
        }
        srand(time(NULL) + getpid());
        simulate(n,semaphore,nhits);
        exit(0);
    }
    
    wait(NULL);
    wait(NULL);


    // pid_t pids[NUM_PROCS];
    // for (int i = 0; i < NUM_PROCS; i++) {
    //     pids[i] = fork();
    //     if (pids[i] == 0) { 
    //         srand(time(NULL) + getpid());
    //         simulate(n, semaphore, nhits);
    //         exit(0); 
    //     }
    // }

    // for (int i = 0; i < NUM_PROCS; i++) {
    //     waitpid(pids[i], NULL, 0);
    // }

    double probability = (double)*nhits / (NUM_PROCS * NUM_TRIALS);
    printf("Estimated probability for %d students: %.2f%%.\n", n, probability * 100);

    sem_destroy(semaphore);
    munmap(semaphore, sizeof(sem_t));
    munmap(nhits, sizeof(int));

    return 0;
}

