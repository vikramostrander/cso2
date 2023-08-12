/*
    COMMANDS
    gcc -lm -fopenmp -O2 test.c
    ./a.out /usr/bin/emacs-24.3

    Even, Many-to-few is fastest
        - SPLIT: More overhead on task queue, unnecessary with quick math operations
        - REDUCE: Many-to-few much faster than atomic

    Other versions not faster than original because extra time from malloc-ing array
*/

#include <stdio.h> // fopen, fread, fclose, printf, fseek, ftell
#include <math.h> // log, exp
#include <stdlib.h> // free, realloc
#include <time.h> // struct timespec, clock_gettime, CLOCK_REALTIME
#include <errno.h>


// computes the geometric mean of a set of values.
// You should use OpenMP to make faster versions of this.
// Keep the underlying sum-of-logs approach.
double geomean(unsigned char *s, size_t n) {
    double answer = 0;
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer += log(s[i]) / n;
    }
    return exp(answer);
}


// even split, atomic reduction
double geomean1(unsigned char *s, size_t n) {
    double answer = 0;
    double *array = malloc(sizeof(double) * n);

    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) array[i] = log(s[i]) / n;
        else array[i] = 0;
    }

    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        #pragma omp atomic update
        answer += array[i];
    }

    free(array);

    return exp(answer);
}

// even split, mant-to-few reduction
double geomean2(unsigned char *s, size_t n) {
    double answer = 0;
    double *array = malloc(sizeof(double) * n);

    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) array[i] = log(s[i]) / n;
        else array[i] = 0;
    }

    #pragma omp parallel
    {
        double local_answer = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            local_answer += array[i];
        }
        
        #pragma omp atomic update
        answer += local_answer;
    }

    free(array);

    return exp(answer);
}

// task queue, atomic reduction
double geomean3(unsigned char *s, size_t n) {
    double answer = 0;
    double *array = malloc(sizeof(double) * n);

    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) array[i] = log(s[i]) / n;
        else array[i] = 0;
    }

    #pragma omp parallel
    {
        double local_answer = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            local_answer += array[i];
        }
        
        #pragma omp atomic update
        answer += local_answer;
    }

    free(array);

    return exp(answer);
}

// task queue, many-to-few reduction
double geomean4(unsigned char *s, size_t n) {
    double answer = 0;
    double *array = malloc(sizeof(double) * n);

    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) array[i] = log(s[i]) / n;
        else array[i] = 0;
    }

    #pragma omp parallel
    {
        double local_answer = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            local_answer += array[i];
        }
        
        #pragma omp atomic update
        answer += local_answer;
    }

    free(array);

    return exp(answer);
}

/// nanoseconds that have elapsed since 1970-01-01 00:00:00 UTC
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}


/// reads arguments and invokes geomean; should not require editing
int main(int argc, char *argv[]) {
    // step 1: get the input array (the bytes in this file)
    char *s = NULL;
    size_t n = 0;
    for(int i=1; i<argc; i+=1) {
        // add argument i's file contents (or string value) to s
        FILE *f = fopen(argv[i], "rb");
        if (f) { // was a file; read it
            fseek(f, 0, SEEK_END); // go to end of file
            size_t size = ftell(f); // find out how many bytes in that was
            fseek(f, 0, SEEK_SET); // go back to beginning
            s = realloc(s, n+size); // make room
            fread(s+n, 1, size, f); // append this file on end of others
            fclose(f);
            n += size; // not new size
        } else { // not a file; treat as a string
            errno = 0; // clear the read error
        }
    }

    // step 2: invoke and time the geometric mean function
    long long t0 = nsecs();
    double answer = geomean(s,n);
    long long t1 = nsecs();

    // step 3: report result
    printf("GEOMEAN0: %lld ns to process %zd characters: %g\n", t1-t0, n, answer);

    // geomean1
    t0 = nsecs();
    answer = geomean1(s,n);
    t1 = nsecs();

    printf("GEOMEAN1: %lld ns to process %zd characters: %g\n", t1-t0, n, answer);

    // geomean2
    t0 = nsecs();
    answer = geomean2(s,n);
    t1 = nsecs();

    printf("GEOMEAN2: %lld ns to process %zd characters: %g\n", t1-t0, n, answer);

    // geomean3
    t0 = nsecs();
    answer = geomean3(s,n);
    t1 = nsecs();

    printf("GEOMEAN3: %lld ns to process %zd characters: %g\n", t1-t0, n, answer);

    // geomean4
    t0 = nsecs();
    answer = geomean4(s,n);
    t1 = nsecs();

    printf("GEOMEAN4: %lld ns to process %zd characters: %g\n", t1-t0, n, answer);
    
    free(s);
}