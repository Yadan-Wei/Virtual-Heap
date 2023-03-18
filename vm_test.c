#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "virtualHeap.h"

// Global variables 
const int BUFFER_SIZE = 50;
const int BLOCKS_NUM = 12;
const int THREADS_NUM = 12;
virtual_t *VIRT_BLOCKS[BLOCKS_NUM];

// Function prototypes
void init_virt_blocks();
void print_virt_blocks();
void alloc_one_virtual_mem_test(int index);
void free_one_virtual_mem_test(int index);
void alloc_full_virtual_mem_test();
void free_full_virtual_mem_test();
void simple_write_read_test(int index);
void full_mem_write_test();
void full_mem_read_test(int index);


void *thread_alloc_routine(void *arg);
void *thread_free_routine(void *arg);
void *thread_write_read_routine(void *arg);
void thread_alloc_test();
void thread_free_test();
void thread_write_read_test();

int main()
{
    pm_init();


    // Setup a global array for virtual blocks
    init_virt_blocks();
    
    printf("\n***** Start testing for main thread *****\n");
    printf("Before allocation: \n");
    print_virt_blocks();

    int test_block = 0;
    // test for single pm_malloc()
    alloc_one_virtual_mem_test(test_block);
    print_virt_blocks();

    // test for single pm_free()
    free_one_virtual_mem_test(test_block);
    print_virt_blocks();
    
    // test for pm_malloc() to allocate all virtual memory blocks
    alloc_full_virtual_mem_test();
    print_virt_blocks();
    
    // test for pm_free() to free all allocated virtual memory blocks
    free_full_virtual_mem_test();
    print_virt_blocks();
    
    // test for pm_write() to write to one virtual memory block
    simple_write_read_test(test_block);

    // test for pm_write() to write to all virtual memory block
    full_mem_write_test();

    // test for pm_read() to read swapped out virtual memory blocks
    int read_vb1 = 1;
    int read_vb2 = 2;
    full_mem_read_test(read_vb1);
    full_mem_read_test(read_vb2);

    // free all virtual memory after done with main thread tests
    free_full_virtual_mem_test();


    // Test for read and write with multi-threads
    printf("\n***** Start testing for multi-threading processes *****\n");
    init_virt_blocks();

    // Test for malloc with multi-threads 
    thread_alloc_test();

    printf("\nTest for multi-threading write and read:\n");
    thread_write_read_test();

    // Test for free with multi-threads
    printf("\nTest for multi-threading free:\n");
    thread_free_test();

    return 0;
}

/*
Initialize the array of virtual_t type to NULL before testing
*/
void init_virt_blocks() {
    int i;
    for (i = 0; i < BLOCKS_NUM; i++) {
        VIRT_BLOCKS[i] = NULL;
    }
}


void alloc_one_virtual_mem_test(int index) {
    printf("After allocating vBlock #%d: \n", index);
    VIRT_BLOCKS[index] = pm_malloc(PAGE_SIZE);
    assert(NULL != VIRT_BLOCKS[index]);
}

void free_one_virtual_mem_test(int index) {
    printf("After freeing vBlock: %d\n", index);
    pm_free(VIRT_BLOCKS[index]);
}

void alloc_full_virtual_mem_test() {
    printf("After allocating full memory blocks:\n");
    int i;
    for (i = 0; i < BLOCKS_NUM; i++) {
        VIRT_BLOCKS[i] = pm_malloc(PAGE_SIZE);
    }
}

void free_full_virtual_mem_test() {
    printf("Done with freeing full memory blocks:\n");
    int i;
    for (i = 0; i < BLOCKS_NUM; i++) {
        if (VIRT_BLOCKS[i]){
            pm_free(VIRT_BLOCKS[i]);
        }
    }
}

void simple_write_read_test(int test_block) {
    char string[BUFFER_SIZE] = "simple_write_read_test";  // string to write
    // allocate one block
    VIRT_BLOCKS[test_block] = pm_malloc(PAGE_SIZE);
    // write to allocated test_block
    pm_write(VIRT_BLOCKS[test_block], string, strlen(string));
    // print out the result
    printf("After write to vblock #%d:\n", test_block);
    print_virt_blocks();

    // free allocated memory block after done with test
    pm_free(VIRT_BLOCKS[test_block]);
    printf("Done with write test, vBlock #%d is free\n\n", test_block);
}

void full_mem_write_test() {
    char string[BUFFER_SIZE];
    int size = 0;
    int i;
    for (i = 0; i < BLOCKS_NUM; i++) {
        VIRT_BLOCKS[i] = pm_malloc(PAGE_SIZE);
        sprintf(string, "full_mem_write_test_%d", i);
        size = strlen(string);
        // write to a block
        if (VIRT_BLOCKS[i]) {
            pm_write(VIRT_BLOCKS[i], string, size);
        }
    }
    printf("After writing to all memory blocks:\n");
    print_virt_blocks();
}

void full_mem_read_test(int test_block) {
    // read swapped blocks on virtual memory
    if (!VIRT_BLOCKS[test_block]) {
        printf("full_mem_read_test fail: block #%d is null.", test_block);
    }
    pm_read(VIRT_BLOCKS[test_block]);
    printf("After reading vblock #%d:\n", test_block);
    print_virt_blocks();
}


void print_virt_blocks() {
    // Header for virt_blocks table
    char header[BUFFER_SIZE] = "vBlock used swapped pBock string";

    int used, swapped, pblock;
    char line[BUFFER_SIZE];
    int i;

    printf("-----------------------------------------\n");
    printf("%s\n", header);
    for (i = 0; i < BLOCKS_NUM; i++) {
        // set line as empty string
        memset(line, 0, BUFFER_SIZE);  

        if (VIRT_BLOCKS[i] == NULL) {  // if the block is empty
            used = 0;
            swapped = 0;
            pblock = -1;
        } else {    // if the block is occupied
            used = VIRT_BLOCKS[i]->used;
            swapped = VIRT_BLOCKS[i]->swapped;
            pblock = get_page_num(VIRT_BLOCKS[i]);
            if (VIRT_BLOCKS[i]->physical) {
                char *paddr = pm_read(VIRT_BLOCKS[i]);
                sscanf(paddr, "%s", line);
            }
        }
        printf("%5d%5d%5d%8d%25s\n", i, used, swapped, pblock, line);
    }
    printf("-----------------------------------------\n\n");
}

/*
Test pm_malloc() function is thread-safe. 
Here we allocate the virtual memory with multiple threads.
If the memory is allocated successfully, assign each allocated the vritual memory 
to the global variable, VIRT_BLOCKS array.
*/
void thread_alloc_test() {
    pthread_t threads[THREADS_NUM];
    int i, j;

    for (i = 0; i < THREADS_NUM; i++) {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_alloc_routine, index);
    }

    for (j = 0; j < THREADS_NUM; j++) {
        pthread_join(threads[j], NULL);
    }
}

/*
Test pm_free function is thread-safe.
After allocating virtual memory in the virutal blocks, 
*/
void thread_free_test() {
    pthread_t threads[THREADS_NUM];
    int i, j;

    for (i = 0; i < THREADS_NUM; i++) {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_free_routine, index);
    }
    for (j = 0; j < THREADS_NUM; j++) {
        pthread_join(threads[j], NULL);
    }
    printf("All blocks are free!\n");
}

/*
Test read and write is thread-safe.
Create multiple threads and call the write and read functions.
*/
void thread_write_read_test() {
    pthread_t threads[THREADS_NUM];
    int i, j;

    for (i = 0; i < THREADS_NUM; i++) {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_write_read_routine, index);
    }

    for (j = 0; j < THREADS_NUM; j++) {
        pthread_join(threads[j], NULL);
    }
}

/*
In this routine function, each thread can write to the virutal memory,
or read from the virtual memory concurrently.
*/
void *thread_write_read_routine(void *arg) {
    int block = *(int *)arg;  // block number

    // Write to the physical memory
    char str[BUFFER_SIZE];
    sprintf(str, "Hello from virtual block %d", block);
    int size = strlen(str);

    if (VIRT_BLOCKS[block]) {
        pm_write(VIRT_BLOCKS[block], str, size);
        printf("Thread %d done with writing...\n", block);
    }

    // Read from the physical memory
    char read_line[BUFFER_SIZE];
    sprintf(read_line, "virt_block_%d read: ", block);
    int i;

    if (VIRT_BLOCKS[block]) {
        char *paddr = pm_read(VIRT_BLOCKS[block]);
        printf("%s ", read_line);
        for (i = 0; i < size; i++) {
            putchar(*paddr++);
        }
        printf("\n");
    }
    return NULL;
}

/*
In this routine, each thread can allocate virtual memory for VIRT_BLOCKS
*/
void *thread_alloc_routine(void *arg) {
    int block = *(int *)arg;
    VIRT_BLOCKS[block] = pm_malloc(PAGE_SIZE);
    if (!VIRT_BLOCKS[block]) {
        printf("Thread %d: virt_block_%d fails to allocate. Virtual memory is full.\n", block, block);
        return NULL;
    }
    int page_num = get_page_num(VIRT_BLOCKS[block]);
    printf("virt_block_%d allocated page #%d\n", block, page_num);

    free(arg);
    return NULL;
}

/*
In this routing, each thread can free the allocated memory at VIRT_BLOCKS
*/
void *thread_free_routine(void *arg) {
    int tid = *(int *)arg;
    if (VIRT_BLOCKS[tid]) {  // if the virtual block is not empty
        pm_free(VIRT_BLOCKS[tid]);
        assert(0 == VIRT_BLOCKS[tid]->used);
    }
    return NULL;
}