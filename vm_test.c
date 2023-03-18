#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

#include "virtualHeap.h"

virtual_t *memories[VIRTUAL_BLOCK_NUM];

void *thread_write_read_test(void *arg)
{
    int pid = *(int *)arg; // thread id
    virtual_t *vptr = pm_malloc(PAGE_SIZE);
    if (!vptr)
    {
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
    for (int i = 0; i < str_size; i++)
    {
        putchar(*paddr++);
    }
    printf("\n");
    return NULL;
}

void *thread_alloc_test(void *arg)
{
    int pid = *(int *)arg;
    virtual_t *vptr = pm_malloc(PAGE_SIZE);
    if (!vptr)
    {
        printf("Virtual memory is full.\n");
        return NULL;
    }
    int page_num = get_page_num(vptr);
    printf("Thread %d allocates page #%d\n", pid, page_num);

    free(arg);
    return NULL;
}

void test_single_thread_not_swap_malloc_free()
{

    for (int i = 0; i < PHYSICAL_BLOCK_NUM; i++)
    {
        memories[i] = pm_malloc(PAGE_SIZE);
        if (!memories[i])
        {
            printf("Virtual memory is full.\n");
            break;
        }
        printf("%d: Allocate to physical page #%d\n", i, get_page_num(memories[i]));
        printf("%d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
    }

    printf("\n");
    printf("Free memory and check status.\n");

    for (int i = 0; i < PHYSICAL_BLOCK_NUM; i++)
    {
        pm_free(memories[i]);
        printf("%d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
    }
    printf("\n");
}

void test_single_thread_swap_malloc_free()
{
    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        memories[i] = pm_malloc(PAGE_SIZE);
        printf("%d: Allocate to physical page #%d\n", i, get_page_num(memories[i]));
    }

    printf("\n");
    printf("Check status of all allocated memory.\n");

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        printf("%d Status: used %d, swapped: %d, offset: %ld\n", i, memories[i]->used, memories[i]->swapped, memories[i]->offset);
    }

    printf("\n");
    printf("Free memory and check status.\n");

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        pm_free(memories[i]);
        printf("%d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
    }
}

void test_single_thread_swap_write_read()
{

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        memories[i] = pm_malloc(PAGE_SIZE);
        if (!memories[i])
        {
            printf("Virtual memory is full.\n");
            break;
        }
        printf("Write to number %d:\n", i);
        char write_content[100] = "Hello CS5600 from number ";
        char num_str[20];
        sprintf(num_str, "%d", i);
        strcat(write_content, num_str);
        pm_write(memories[i], write_content);
    }

    printf("\n");
    printf("Read from file or memory.\n");

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        if (!memories[i])
        {
            printf("Virtual memory is full.\n");
            break;
        }

        printf("Before Read %d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
        printf("Read content is: %s\n", pm_read(memories[i]));
        printf("After read %d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
        printf("\n");
    }

    printf("\n");
    printf("Free memory and check status.\n");

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        pm_free(memories[i]);
        printf("%d Status: used %d, swapped: %d\n", i, memories[i]->used, memories[i]->swapped);
    }
}

void test_single_thread_memory_full()
{
    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        memories[i] = pm_malloc(PAGE_SIZE);
        if (!memories[i])
        {
            printf("Virtual memory is full.\n");
            break;
        }
        printf("%d: Allocate to physical page #%d\n", i, get_page_num(memories[i]));
    }

    virtual_t *v11 = pm_malloc(PAGE_SIZE);
    if (v11)
    {
        printf("The virtual memory is not full.\n");
    }
    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        pm_free(memories[i]);
    }
}

int main()
{
    pm_init();

    printf("\n\n");
    printf("***********Test single thread not swap malloc and free.*********\n");
    test_single_thread_not_swap_malloc_free();

    printf("\n\n");
    printf("***********Test single thread allocate more than physcial, swap malloc and free.*******\n");
    test_single_thread_swap_malloc_free();

    printf("\n\n");
    printf("***********Test single thread read and write.***********\n");
    test_single_thread_swap_write_read();

    printf("\n\n");
    printf("************Test single thread memory is exhausted.***********\n");
    test_single_thread_memory_full();

    // Test for malloc and free with multi-threads
    printf("Test for multi-threading malloc and free:\n");
    int threads_num = 12;
    pthread_t threads[threads_num];

    for (int i = 0; i < threads_num; i++)
    {
        int *index = (int *)malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &thread_alloc_test, index);
    }

    for (int i = 0; i < threads_num; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Test for read and write with multi-threads
    // printf("\nTest for multi-threading write and read:\n");
    // for (int i = 0; i < threads_num; i++) {
    //     int *index = (int *)malloc(sizeof(int));
    //     *index = i;
    //     pthread_create(&threads[i], NULL, &thread_write_read_test, index);
    // }

    // for (int i = 0; i < threads_num; i++) {
    //     pthread_join(threads[i], NULL);
    // }

    return 0;
}