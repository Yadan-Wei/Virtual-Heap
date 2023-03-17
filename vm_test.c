#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

#include "virtualHeap.h"

void *thread_write_read_test(void *arg) {
    int pid = *(int *)arg;  // thread id
    virtual_t *vptr = pm_malloc(PAGE_SIZE);
    if (!vptr) {
        printf("Virtual memory is full.\n");
        return NULL;
    }

    // Write to the physical memory
    char *str = "Thread ";
    char cid = pid + '0';
    strcat(str, &cid);
    
    pm_write(vptr, str);

    // Read from the physical memory
    char *paddr = pm_read(vptr);
    int str_size = strlen(str);
    printf("Read from ");
    for (int i = 0; i < str_size; i++) {
        putchar(*paddr++);
    }
    printf("\n");
    return NULL;
}

void *thread_alloc_test(void *arg) {
    int pid = *(int *)arg;
    virtual_t *vptr = pm_malloc(PAGE_SIZE);
    if (!vptr) {
        printf("Virtual memory is full.\n");
        return NULL;
    }
    int page_num = get_page_num(vptr);
    printf("Thread %d allocates page #%d\n", pid, page_num);

    free(arg);
    return NULL;
}

int main()
{
    pm_init();

    // Test for malloc and free in main thread
    printf("******Test pm_malloc can assign more memory than actual physcial volume.*********\n");
    virtual_t *v1 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v1 allocated physcial address is %p\n", v1->physical->physical_addr);
    printf("v1 used: %d, v1 swapped: %d\n", v1->used, v1->swapped);
    pm_check(v1);
    char *a = (char *)(v1->physical->physical_addr);
    strcpy(a, "Hello World!");
    printf("the value assigned to v1 is %s\n", (char *)(v1->physical->physical_addr));

    virtual_t *v2 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v2 allocated physcial address is %p\n", v2->physical->physical_addr);
    printf("v2 used: %d, v2 swapped: %d\n", v2->used, v2->swapped);

    pm_check(v2);
    char *b = (char *)(v2->physical->physical_addr);
    strcpy(b, "Hello CS5600!");
    printf("the value assigned to v2 is %s\n", (char *)(v2->physical->physical_addr));

    virtual_t *v3 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v3 allocated physcial address is %p\n", v3->physical->physical_addr);
    printf("v3 used: %d, v3 swapped: %d\n", v3->used, v3->swapped);
    virtual_t *v4 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v4 allocated physcial address is %p\n", v4->physical->physical_addr);
    printf("v4 used: %d, v4 swapped: %d\n", v4->used, v4->swapped);
    virtual_t *v5 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v5 allocated physcial address is %p\n", v5->physical->physical_addr);
    printf("v5 used: %d, v5 swapped: %d\n", v5->used, v5->swapped);
    virtual_t *v6 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v6 allocated physcial address is %p\n", v6->physical->physical_addr);
    printf("v6 used: %d, v6 swapped: %d\n", v6->used, v6->swapped);
    virtual_t *v7 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v7 allocated physcial address is %p\n", v7->physical->physical_addr);
    printf("v7 used: %d, v7 swapped: %d\n", v7->used, v7->swapped);

    printf("*******After allocate v6, v7 check v1, v2 is swapped******\n");
    printf("v1 used: %d, v1 swapped: %d, v1 offset: %ld\n", v1->used, v1->swapped, v1->offset);
    printf("v2 used: %d, v2 swapped: %d, v2 offset: %ld\n", v2->used, v2->swapped, v2->offset);

    printf("******Test reuse v1, v2 and swap in v1, v2 from file.*****\n");
    pm_check(v1);
    printf("v1 used: %d, v1 swapped: %d\n", v1->used, v1->swapped);
    printf("the value assigned to v1 is %s\n", (char *)(v1->physical->physical_addr));

    pm_check(v2);
    printf("v2 used: %d, v2 swapped: %d\n", v2->used, v2->swapped);
    printf("the value assigned to v2 is %s\n", (char *)(v2->physical->physical_addr));

    printf("******Test pm_free() ******\n");

    printf("Before free .....\n");
    printf("v1 used: %d, v1 swapped: %d\n", v1->used, v1->swapped);
    printf("v3 used: %d, v3 swapped: %d\n", v3->used, v3->swapped);
    printf("After free .....\n");
    pm_free(v1);
    printf("v1 used: %d, v1 swapped: %d\n", v1->used, v1->swapped);
    pm_free(v3);
    printf("v3 used: %d, v3 swapped: %d\n", v3->used, v3->swapped);

    virtual_t *v8 = pm_malloc(4 * 1024 * sizeof(char));
    printf("v8 allocated physcial address is %p\n", v8->physical->physical_addr);
    printf("v8 used: %d, v8 swapped: %d\n", v8->used, v8->swapped);
    printf("\n");

    // free allocated memory
    pm_free(v2);
    pm_free(v4);
    pm_free(v5);
    pm_free(v6);
    pm_free(v7);
    pm_free(v8);
    
    // Test for malloc and free with multi-threads 
    printf("Test for multi-threading malloc and free:\n");
    int threads_num = 12;
    pthread_t threads[threads_num];

    for (int i = 0; i < threads_num; i++) {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_alloc_test, index);
    }

    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }


    // Test for read and write with multi-threads
    printf("\nTest for multi-threading write and read:\n");
    for (int i = 0; i < threads_num; i++) {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_write_read_test, index);
    }

    for (int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}