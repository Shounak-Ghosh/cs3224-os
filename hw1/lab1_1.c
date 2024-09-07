#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    printf("Hello world! This is CS3224, Fall 2024!\n");

    printf("Shounak Ghosh\n");  

    srand(time(NULL));
    int random_number = rand() % 200;
    printf("%d\n", random_number);

    
    return 0;
}
