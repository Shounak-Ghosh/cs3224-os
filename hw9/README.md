Compile both programs using `gcc`:

```
gcc -o producer producer.c -lrt -lpthread
gcc -o consumer consumer.c -lrt -lpthread
```

In the first terminal, start the producer with the desired Fibonacci sequence length (eg. `10`). 
```
./producer 10
```

In the second terminal, start the consumer with the same Fibonacci sequence length:

```
./consumer 10
```

Cleanup:
Ensure the shared memory and semaphores are cleaned up after the programs finish.
```
shm_unlink /lab9_shm
sem_unlink /sem_empty
sem_unlink /sem_full
sem_unlink /sem_mutex
```