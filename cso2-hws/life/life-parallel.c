#include "life.h"
#include <pthread.h>

typedef struct {
    pthread_t thread;
    pthread_barrier_t *barrier;
    int start;
    int end;
    LifeBoard *state;
    LifeBoard *next_state;
    int steps;

    int index;
} ThreadInfo;

LifeCell is_alive(LifeBoard *state, int x, int y) {

    int live_in_window = 0;
        for (int y_offset = -1; y_offset <= 1; y_offset += 1)
            for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                if (LB_get(state, x + x_offset, y + y_offset))
                    live_in_window += 1;

    return live_in_window == 3 || (live_in_window == 4 && LB_get(state, x, y));

}

void *thread_simulate(void *arguments) {

    ThreadInfo *info = (ThreadInfo *)arguments;

    for (int step = 0; step < info->steps; ++step) {

        //printf("thread %d goes %d, %d\n", info->index, info->start, info->end);

        for (int y = info->start; y < info->end; y += 1)
            for (int x = 1; x < info->state->width - 1; x += 1)
                LB_set(info->next_state, x, y, is_alive(info->state, x, y));

        //printf("thread %d waiting for barriers\n", info->index);
        pthread_barrier_wait(info->barrier);
        if(info->index == 0) LB_swap(info->state, info->next_state);
        pthread_barrier_wait(info->barrier);
        //printf("thread %d past barriers\n", info->index);

    }

    return NULL;

}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {

    //printf("number of threads is %d\n", threads);
    
    LifeBoard *next_state = LB_new(state->width, state->height);

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, threads);

    ThreadInfo info[threads];
    int thread_height = state->height / threads;

    //printf("total height is %d\n", state->height);
    //printf("thread height is %d\n", thread_height);

    for (int i = 0; i < threads; ++i) {
        info[i].barrier = &barrier;
        info[i].start = i * thread_height;
        info[i].end = (i + 1) * thread_height;
        info[i].state = state;
        info[i].next_state = next_state;
        info[i].steps = steps;
        info[i].index = i;
    }
    info[0].start = 1;
    info[threads - 1].end = state->height - 1;

    //for (int i = 0; i < threads; ++i) 
    //    printf("thread %d ranges [%d, %d)\n", info[i].index, info[i].start, info[i].end);

    for (int i = 0; i < threads; ++i)
        pthread_create(&info[i].thread, NULL, thread_simulate, (void *)&info[i]);

    for (int i = 0; i < threads; ++i)
        pthread_join(info[i].thread, NULL);

    pthread_barrier_destroy(&barrier);
    LB_del(next_state);

}