#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define PHYSICAL_SIZE 10 * 1024 * 1024
#define PAGE_SIZE 4 * 1024
#define FILEPATH "./memory.dat"

const int virtual_size = 2 * PHYSICAL_SIZE;
const int virtual_block_num = virtual_size / PAGE_SIZE;
const int physical_block_num = PHYSICAL_SIZE / PAGE_SIZE;

typedef struct physical_t
{
    int used;
    size_t size;
    void *physical_addr;
} physical_t;

typedef struct virtual_t
{
    int swapped;
    int used;
    int offset;
    physical_t *physical;
    size_t size;
} virtual_t;

static unsigned char pm_heap[PHYSICAL_SIZE];
static virtual_t pm_virtual[virtual_block_num];
static physical_t pm_physical[physical_block_num];

int physical_available = physical_block_num;
int virtual_available = virtual_block_num;
int file_current_offset = 0;

// init all physical blocks point to physical address

void pm_init()
{

    for (int i = 0; i < physical_block_num; i++)
    {
        physical_t *physical_block;
        physical_block->size = PAGE_SIZE;
        physical_block->used = 0;
        physical_block->physical_addr = (void *)pm_heap[i * PAGE_SIZE];
        pm_physical[i] = physical_block;
    }
}

/*
Check if physical is exhausted.
*/
int is_physical_full()
{
    if (physical_available == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
Check if virtual is exhausted.
*/
int is_virtual_full()
{
    if (virtual_available == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
Malloc funtcion return a pointer to virtual block
*/
virtual_t *pm_malloc(size_t size)
{
    if (is_virtual_full())
    {
        return 0;
    }
    virtual_t *virtual_block = find_first_available_virtual_block();
    virtual_block->used = 1;
    virtual_block->size = size;
    virtual_available -= 1;
    return virtual_block;
}

/*
Find the first available virtual block to assign, and map it with physical block accordingly.
*/
virtual_t *find_first_available_virtual_block()
{
    for (int i = 0; i < virtual_block_num; i++)
    {
        virtual_t *virtual_block = pm_virtual[i];
        if (!virtual_block->used)
        {
            map_physical_virtual(virtual_block);
            return virtual_block;
        }
    }
}

/*
Helper function that map physical block and virtual block, if physical is not full, find the first unused
else swap out the first used and not swapped virtual block's physical and use it.(FIFO)
*/
void map_physical_virtual(virtual_t *virtual_block)
{
    physical_t *physical_block;
    if (is_physical_full())
    {
        physical_block = swap_out();
    }
    else
    {
        physical_block = find_first_available_physical_block();
        physical_block->used = 1;
        physical_available -= 1;
    }
    virtual_block->physical = physical_block;
}

/*
Find first unused physical block.
*/
physical_t *find_first_available_physical_block()
{

    for (int i = 0; i < physical_block_num; i++)
    {
        physical_t *physical_block = pm_physical[i];
        if (!physical_block->used)
        {
            return physical_block;
        }
    }
}

/*
Swap out the first used but not swapped virtual block's physical block and return it.
*/
physical_t *swap_out()
{
    virtual_t *swap_out_block = find_first_used_not_swapped_virtual_block();
    physical_t *swap_out_physical_block = swap_out_block->physical;

    FILE *fp = fopen(FILEPATH, "wb");
    char *buffer = swap_out_physical_block->physical_addr;
    fwrite(buffer, swap_out_block->size, file_current_offset, fp);
    fclose(fp);
    swap_out_block->offset = file_current_offset;
    file_current_offset += 1;
    swap_out_block->physical = 0;
    swap_out_block->swapped = 1;
    return swap_out_physical_block;
}

/*
Find first used but not swapped virtual block.
*/
virtual_t *find_first_used_not_swapped_virtual_block()
{
    for (int i = 0; i < virtual_block_num; i++)
    {
        virtual_t *virtual_block = pm_virtual[i];
        if (virtual_block->used && !virtual_block->swapped)
        {
            return virtual_block;
        }
    }
}

/*
Check if a virtual block is swapped or not, if swapped, swap in else do nothing, return the physcial address that can be used.
*/
void *pm_check(virtual_t *virtual_block)
{
    if (virtual_block->swapped)
    {
        swap_in(virtual_block);
    }
    return virtual_block->physical->physical_addr;
}

/*
Swap in function, swap in a virtual block memory from file.
*/
void swap_in(virtual_t *virtual_block)
{
    virtual_block->physical = swap_out();
    FILE *fp = fopen(FILEPATH, "wb");
    fread(virtual_block->physical->physical_addr, virtual_block->size, virtual_block->offset, fp);
    fclose(fp);
    virtual_block->swapped = 0;
    virtual_block->offset = 0;
}

/*
Free used memory from file or physical.
*/
void pm_free(virtual_t *virtual_block)
{

    if (virtual_block->swapped)
    {
        virtual_block->swapped = 0;
        virtual_block->offset = 0;
    }
    else
    {
        virtual_block->physical->used = 0;
        virtual_block->physical = 0;
        physical_available += 1;
    }
    virtual_block->used = 0;
    virtual_available += 1;
}
