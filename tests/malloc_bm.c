#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE (500 * 1024)
#define ITERATIONS 10

static inline long diff_ns(struct timespec s, struct timespec e) {
    return (e.tv_sec - s.tv_sec) * 1000000000L +
           (e.tv_nsec - s.tv_nsec);
}

int main() {
    struct timespec s, e;

    // ----------------------------------------------------
    // Test 1:
    // Allocate/free every iteration
    // ----------------------------------------------------
    long total1 = 0;

    for (int i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &s);

        void *p = malloc(SIZE);

        clock_gettime(CLOCK_MONOTONIC, &e);

        total1 += diff_ns(s, e);

        free(p);
    }

    double avg1 = (double)total1 / ITERATIONS;

    printf("Allocate inside loop:\n");
    printf("Average malloc time = %.2f ns\n\n", avg1);

    // ----------------------------------------------------
    // Test 2:
    // Allocate once outside loop
    // ----------------------------------------------------
    clock_gettime(CLOCK_MONOTONIC, &s);

    void *p = malloc(SIZE);

    clock_gettime(CLOCK_MONOTONIC, &e);

    long alloc_once = diff_ns(s, e);

    // simulate loop work
    for (int i = 0; i < ITERATIONS; i++) {
        volatile char *x = (char *)p;
        x[0] = (char)i;
    }

    free(p);

    printf("Allocate once outside loop:\n");
    printf("Single malloc time = %ld ns\n", alloc_once);

    return 0;
}