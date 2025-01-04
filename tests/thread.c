#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "include.h"

// Replace with your custom malloc implementation
#define malloc my_malloc
#define free my_free
#define realloc my_realloc

#define NUM_THREADS 1
#define NUM_ALLOCATIONS 10

void *thread_function(void *arg) {
    int thread_id = *(int *)arg;

    printf("Thread %d started\n", thread_id);

    void *pointers[NUM_ALLOCATIONS];

    // Allocate memory in a loop
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % 256) + 1; // Random size between 1 and 256
        pointers[i] = malloc(size);
        assert(pointers[i] != NULL);
        memset(pointers[i], thread_id, size); // Fill memory with thread ID
    }

    // Free half of the allocated memory
    for (int i = 0; i < NUM_ALLOCATIONS / 2; i++) {
        free(pointers[i]);
    }

    // Reallocate the remaining memory
    for (int i = NUM_ALLOCATIONS / 2; i < NUM_ALLOCATIONS; i++) {
        size_t new_size = (rand() % 512) + 1; // Random new size between 1 and 512
        pointers[i] = realloc(pointers[i], new_size);
        assert(pointers[i] != NULL);
        memset(pointers[i], thread_id, new_size); // Fill new memory with thread ID
    }

    // Free all remaining memory
    for (int i = NUM_ALLOCATIONS / 2; i < NUM_ALLOCATIONS; i++) {
        free(pointers[i]);
    }

    printf("Thread %d finished\n", thread_id);
    return NULL;
}

int main() {
    printf("Starting thread safety test...\n");

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        int ret = pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
        assert(ret == 0); // Ensure thread creation succeeded
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed successfully. Thread safety test passed!\n");

    return 0;
}
