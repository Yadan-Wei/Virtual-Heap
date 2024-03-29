#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>

#include "virtualHeap.h"

pthread_mutex_t vm_lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned char pm_heap[PHYSICAL_SIZE];       // pre-allocated physcial memory
static virtual_t pm_virtual[VIRTUAL_BLOCK_NUM];    // virutal memory table
static physical_t pm_physical[PHYSICAL_BLOCK_NUM]; // physical memory table

static int physical_available = PHYSICAL_BLOCK_NUM;
static int virtual_available = VIRTUAL_BLOCK_NUM;

/*
Init physical blocks point to physical address
init all virtual blocks
Clean disk file.
*/
void pm_init()
{
    // open file in write mode
    FILE *fp = fopen(FILEPATH, "w");
    if (fp == NULL)
    {
        printf("Failed to open file.\n");
        exit(1);
    }

    // write an empty string to the file
    // so that the file is initialized
    fwrite("", 1, 0, fp);

    // close the file
    fclose(fp);
    int i;

    // initial physical block array
    for (int i = 0; i < PHYSICAL_BLOCK_NUM; i++)
    {
        pm_physical[i].size = PAGE_SIZE;
        pm_physical[i].used = 0;
        pm_physical[i].physical_addr = (void *)(pm_heap + i * PAGE_SIZE);
    }

    // initial virtual block array
    for (i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        pm_virtual[i].size = 0;
        pm_virtual[i].used = 0;
        pm_virtual[i].physical = NULL;
        pm_virtual[i].swapped = 0;
        pm_virtual[i].offset = 0;
    }
}

/*
Check if physical is exhausted.
*/
int is_physical_full()
{
    return (physical_available == 0);
}

/*
Check if virtual is exhausted.
*/
int is_virtual_full()
{
    return (virtual_available == 0);
}

/*
Find first unused physical block.
*/
physical_t *find_first_available_physical_block()
{
    physical_t *res = NULL;
    int i;
    for (i = 0; i < PHYSICAL_BLOCK_NUM; i++)
    {
        if (!pm_physical[i].used)
        {
            res = &(pm_physical[i]);
            break;
        }
    }
    return res;
}

/*
Find first used but not swapped virtual block.
*/
virtual_t *find_first_used_not_swapped_virtual_block()
{
    virtual_t *res = NULL;
    int i;
    for (i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {

        if (pm_virtual[i].used && !pm_virtual[i].swapped)
        {
            res = &(pm_virtual[i]);
            break;
        }
    }
    return res;
}

/*
Swap out the first used but not swapped virtual block's physical block and return it.
*/
physical_t *swap_out()
{
    virtual_t *swap_out_block = find_first_used_not_swapped_virtual_block();
    physical_t *swap_out_physical_block = swap_out_block->physical;

    FILE *fp = fopen(FILEPATH, "ab");

    if (fp == NULL)
    {
        printf("Failed to open file.\n");
        exit(1);
    }
    char *buffer = (char *)(swap_out_physical_block->physical_addr);

    // move pointer to current end of file
    fseek(fp, 0, SEEK_END);
    // record the offset
    long int offset = ftell(fp);
    // write the content in the memory to the file
    fwrite(buffer, 1, swap_out_block->size, fp);

    fclose(fp);
    // update the status of swap out virtual block
    swap_out_block->offset = offset;
    swap_out_block->physical = NULL;
    swap_out_block->swapped = 1;
    return swap_out_physical_block;
}

/*
Helper function that map physical block and virtual block, if physical is not full, find the first unused
else swap out the first used and not swapped virtual block's physical and use it.(FIFO)
*/
void map_physical_virtual(virtual_t *virtual_block)
{
    physical_t *physical_block;
    // if physical block is full, swap out the first used but not swapped virtual block's physical
    if (is_physical_full())
    {
        physical_block = swap_out();
    }
    // else find a unused physical block
    else
    {
        physical_block = find_first_available_physical_block();
        physical_block->used = 1;
        physical_available -= 1;
    }
    // map physical and virtual
    virtual_block->physical = physical_block;
}

/*
Find the first available virtual block to assign, and map it with physical block accordingly.
*/
virtual_t *find_first_available_virtual_block()
{
    virtual_t *res = NULL;
    int i;
    for (i = 0; i < VIRTUAL_BLOCK_NUM; i++)
    {
        if (!pm_virtual[i].used)
        {
            map_physical_virtual(&(pm_virtual[i]));
            res = &(pm_virtual[i]);
            break;
        }
    }
    return res;
}

/*
Malloc funtcion return a pointer to virtual block
*/
virtual_t *pm_malloc(size_t size)
{
    pthread_mutex_lock(&vm_lock); // thread safety

    // when virtual is full, return NULL
    if (is_virtual_full())
    {
        printf("Virtual memory is full. Please check your memory usage.\n");
        pthread_mutex_unlock(&vm_lock); // unlock the mutex, so other thread can access
        return NULL;
    }
    // find the first unused virtual block and map it with a physical block
    virtual_t *virtual_block = find_first_available_virtual_block();
    virtual_block->used = 1;
    virtual_block->size = size;
    virtual_available -= 1;

    pthread_mutex_unlock(&vm_lock);
    return virtual_block;
}

/*
Swap in function, swap in a virtual block memory from file.
*/
void swap_in(virtual_t *virtual_block)
{
    // if physical is not full, find a unused physical block
    if (!is_physical_full())
    {
        virtual_block->physical = find_first_available_physical_block();
    }
    // else swap out a virtual block and use its physical block
    else
    {
        virtual_block->physical = swap_out();
    }
    FILE *fp = fopen(FILEPATH, "rb+");

    if (fp == NULL)
    {
        printf("Failed to open file.\n");
        exit(1);
    }
    // go to the location where the offset is
    fseek(fp, virtual_block->offset, SEEK_SET);
    // read contents from file
    fread((char *)(virtual_block->physical->physical_addr), 1, virtual_block->size, fp);
    fclose(fp);
    // update virtual block's status
    virtual_block->swapped = 0;
    virtual_block->offset = 0;
}

/*
Check if a virtual block is swapped or not, if swapped, swap in else do nothing.
*/
void pm_check(virtual_t *virtual_block)
{
    if (virtual_block->swapped)
    {
        swap_in(virtual_block);
    }
}

/*
Free used memory from file or physical.
*/
void pm_free(virtual_t *virtual_block)
{
    pthread_mutex_lock(&vm_lock);
    // if virtual block is swapped just update its status
    if (virtual_block->swapped)
    {
        virtual_block->swapped = 0;
        virtual_block->offset = 0;
    }
    // else update its physical block status
    else
    {
        virtual_block->physical->used = 0;
        virtual_block->physical = NULL;
        physical_available += 1;
    }
    // increase available virtual number
    virtual_block->used = 0;
    virtual_available += 1;
    pthread_mutex_unlock(&vm_lock);
}

/*
Write with thread safety
*/
void pm_write(virtual_t *virtual_block, char *string, int size)
{
    pthread_mutex_lock(&vm_lock);

    // check if physical page present
    pm_check(virtual_block);
    // write to the physical address
    char *paddr = virtual_block->physical->physical_addr;
    strncpy(paddr, string, size);

    pthread_mutex_unlock(&vm_lock);
}

/*
Read with thread safety
*/
char *pm_read(virtual_t *virtual_block)
{
    pthread_mutex_lock(&vm_lock);

    // check if physical page present
    pm_check(virtual_block);
    // read starting at the physical address
    char *paddr = (char *)(virtual_block->physical->physical_addr);

    pthread_mutex_unlock(&vm_lock);
    return paddr;
}

/*
Return physical block number that is mapped to a virtual block
*/
int get_page_num(virtual_t *virtual_bock)
{
    int page_num = -1;
    // start physical address
    unsigned char *start = pm_heap;
    // current allocated physical address
    if (virtual_bock->physical)
    {
        unsigned char *current = virtual_bock->physical->physical_addr;
        page_num = (current - start) / PAGE_SIZE;
    }
    return page_num;
}
