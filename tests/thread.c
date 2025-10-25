#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "test_include.h"

// Replace with your custom malloc implementation
#define malloc malloc
#define free free
#define realloc realloc

#define NUM_THREADS 5
#define NUM_ALLOCATIONS 15

void *simple(void *arg) {
    int thread_id = *(int *)arg;
    void *pointers[NUM_ALLOCATIONS] = {NULL};

    printf("\n=== Thread %d started ===\n", thread_id);

    // Allocate memory in a loop
    printf("\nThread %d - Starting allocations:\n", thread_id);
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % 256) + 64;
        pointers[i] = malloc(size);
        if (pointers[i] == NULL) {
            printf("Thread %d - Allocation failed at iteration %d\n", thread_id, i);
        } else {
            printf("Thread %d - Allocated %zu bytes at %p\n", thread_id, size, pointers[i]);
        }
    }

    // Free allocated memory
    printf("\nThread %d - Starting deallocations:\n", thread_id);
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        if (pointers[i] != NULL) {
            free(pointers[i]);
            printf("Thread %d - Freed memory at %p\n", thread_id, pointers[i]);
        }
    }

    printf("\n=== Thread %d finished ===\n", thread_id);
    return NULL;
}

void *thread_function(void *arg) {
    int thread_id = *(int *)arg;
    void *pointers[NUM_ALLOCATIONS] = {NULL};

    printf("\n=== Thread %d started ===\n", thread_id);

    // Allocate memory in a loop
    printf("\nThread %d - Starting allocations:\n", thread_id);
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (rand() % 256) + 64;
        printf("Thread %d - Attempting malloc(%zu) [%d/%d]\n", 
               thread_id, size, i + 1, NUM_ALLOCATIONS);
        
        pointers[i] = malloc(size);
        if (!pointers[i]) {
            printf("Thread %d - ERROR: Malloc failed for allocation %d\n", thread_id, i);
            continue;
        }
        printf("Thread %d - Successfully allocated %zu bytes at %p\n", 
               thread_id, size, pointers[i]);
        
        memset(pointers[i], thread_id, size);
        printf("Thread %d - Memory set completed for allocation %d\n", thread_id, i);
        show_alloc_mem();
    }

    // Free half of the allocated memory
    printf("\nThread %d - Starting to free first half:\n", thread_id);
    for (int i = 0; i < NUM_ALLOCATIONS / 2; i++) {
        if (pointers[i]) {
            printf("Thread %d - Freeing memory at %p [%d/%d]\n", 
                   thread_id, pointers[i], i + 1, NUM_ALLOCATIONS / 2);
            free(pointers[i]);
            pointers[i] = NULL;
            printf("Thread %d - Successfully freed allocation %d\n", thread_id, i);
            show_alloc_mem();
        }
    }

    // Reallocate the remaining memory
    printf("\nThread %d - Starting reallocation of second half:\n", thread_id);
    for (int i = NUM_ALLOCATIONS / 2; i < NUM_ALLOCATIONS; i++) {
        if (pointers[i]) {
            size_t new_size = (rand() % 512) + 64;
            printf("Thread %d - Attempting realloc(%p, %zu) [%d/%d]\n", 
                   thread_id, pointers[i], new_size, i + 1, NUM_ALLOCATIONS);
            
            void *new_ptr = realloc(pointers[i], new_size);
            if (new_ptr) {
                printf("Thread %d - Successfully reallocated from %p to %p\n", 
                       thread_id, pointers[i], new_ptr);
                pointers[i] = new_ptr;
                memset(pointers[i], thread_id, new_size);
                printf("Thread %d - Memory set completed for reallocation %d\n", thread_id, i);
            } else {
                printf("Thread %d - ERROR: Realloc failed for allocation %d\n", thread_id, i);
            }
            show_alloc_mem();
        }
    }

    // Free all remaining memory
    printf("\nThread %d - Starting to free second half:\n", thread_id);
    for (int i = NUM_ALLOCATIONS / 2; i < NUM_ALLOCATIONS; i++) {
        if (pointers[i]) {
            printf("Thread %d - Freeing memory at %p [%d/%d]\n", 
                   thread_id, pointers[i], i + 1, NUM_ALLOCATIONS);
            free(pointers[i]);
            pointers[i] = NULL;
            printf("Thread %d - Successfully freed allocation %d\n", thread_id, i);
            show_alloc_mem();
        }
    }

    printf("\n=== Thread %d finished successfully ===\n", thread_id);
    return NULL;
}

int run_thread_test() {
    printf("\n=== Starting thread safety test with %d threads ===\n", NUM_THREADS);
    printf("Each thread will perform:\n");
    printf("- %d allocations\n", NUM_ALLOCATIONS);
    printf("- Free first %d allocations\n", NUM_ALLOCATIONS / 2);
    printf("- Reallocate remaining %d blocks\n", NUM_ALLOCATIONS / 2);
    printf("- Free all remaining blocks\n\n");

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        printf("Creating thread %d...\n", i);
        int ret = pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
        if (ret != 0) {
            printf("ERROR: Failed to create thread %d (error: %d)\n", i, ret);
            return 1;
        }
        printf("Thread %d created successfully\n", i);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Waiting for thread %d to finish...\n", i);
        pthread_join(threads[i], NULL);
        printf("Thread %d joined successfully\n", i);
    }

    printf("\n=== Thread safety test completed successfully! ===\n\n");
    return 0;
}
