#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define PHYSICAL_SIZE (20 * 1024)
#define PAGE_SIZE (4 * 1024)
#define FILEPATH "memory.dat"

#define VIRTUAL_SIZE (2 * PHYSICAL_SIZE)
#define VIRTUAL_BLOCK_NUM (VIRTUAL_SIZE / PAGE_SIZE)
#define PHYSICAL_BLOCK_NUM (PHYSICAL_SIZE / PAGE_SIZE)

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
    long int offset;
    physical_t *physical;
    size_t size;
} virtual_t;

static unsigned char pm_heap[PHYSICAL_SIZE];
static virtual_t pm_virtual[VIRTUAL_BLOCK_NUM];
static physical_t pm_physical[PHYSICAL_BLOCK_NUM];

int physical_available = PHYSICAL_BLOCK_NUM;
int virtual_available = VIRTUAL_BLOCK_NUM;

// init all physical blocks point to physical address

void pm_init()
{

    for (int i = 0; i < PHYSICAL_BLOCK_NUM; i++)
    {
        pm_physical[i].size = PAGE_SIZE;
        pm_physical[i].used = 0;
        pm_physical[i].physical_addr = (void *)(pm_heap + i * PAGE_SIZE);
    }

    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
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
Find first unused physical block.
*/
physical_t *find_first_available_physical_block()
{
    physical_t *res = NULL;
    for (int i = 0; i < PHYSICAL_BLOCK_NUM; i++)
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
    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
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
    // printf("The buffer value is %s\n", buffer);
    fseek(fp, 0, SEEK_END);
    long int offset = ftell(fp);
    fwrite(buffer, 1, swap_out_block->size, fp);

    // printf("after writing offset is %ld\n", ftell(fp));
    fclose(fp);
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
Find the first available virtual block to assign, and map it with physical block accordingly.
*/
virtual_t *find_first_available_virtual_block()
{
    virtual_t *res = NULL;
    for (int i = 0; i < VIRTUAL_BLOCK_NUM; i++)
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
    if (is_virtual_full())
    {
        return NULL;
    }
    virtual_t *virtual_block = find_first_available_virtual_block();
    virtual_block->used = 1;
    virtual_block->size = size;
    virtual_available -= 1;
    return virtual_block;
}

/*
Swap in function, swap in a virtual block memory from file.
*/
void swap_in(virtual_t *virtual_block)
{
    if (!is_physical_full())
    {
        virtual_block->physical = find_first_available_physical_block();
    }
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
    fseek(fp, virtual_block->offset, SEEK_SET);
    fread((char *)(virtual_block->physical->physical_addr), 1, virtual_block->size, fp);
    fclose(fp);
    virtual_block->swapped = 0;
    virtual_block->offset = 0;
}

/*
Check if a virtual block is swapped or not, if swapped, swap in else do nothing, return the physcial address that can be used.
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

    if (virtual_block->swapped)
    {
        virtual_block->swapped = 0;
        virtual_block->offset = 0;
    }
    else
    {
        virtual_block->physical->used = 0;
        virtual_block->physical = NULL;
        physical_available += 1;
    }
    virtual_block->used = 0;
    virtual_available += 1;
}

int main()
{
    pm_init();
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

    return 0;
}
