#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *eat(void *arg) {
    int n = (int) (long)arg;
    // take two chopsticks
    if(n < (n+1)%5) {
        pthread_mutex_lock(&chopstick[n]);
        printf("Philosopher %d got chopstick %d\n", n, n);
        pthread_mutex_lock(&chopstick[(n+1)%5]);
        printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
    }
    else {
        pthread_mutex_lock(&chopstick[(n+1)%5]);
        printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
        pthread_mutex_lock(&chopstick[n]);
        printf("Philosopher %d got chopstick %d\n", n, n);
    }
    
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}

int main(int argc, const char *argv[]) {
    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i =0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);

    for(int i=0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    return 0;
}