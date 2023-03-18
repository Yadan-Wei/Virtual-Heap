#ifndef VIRTUALHEAP_H
#define VIRTUALHEAP_H

#define PHYSICAL_SIZE (20 * 1024)   // 20KB
#define PAGE_SIZE (4 * 1024)        // 4KB
#define FILEPATH "memory.dat"       // virtual memory

#define VIRTUAL_SIZE (2 * PHYSICAL_SIZE)
#define VIRTUAL_BLOCK_NUM (VIRTUAL_SIZE / PAGE_SIZE)     // 10 pages
#define PHYSICAL_BLOCK_NUM (PHYSICAL_SIZE / PAGE_SIZE)   // 5 pages

/*
Physical table entry 
*/
typedef struct physical_t
{
    int used;
    size_t size;         
    void *physical_addr;  // map to physical memory address
} physical_t;

/*
Virtual table entry
*/
typedef struct virtual_t
{
    int swapped;            // 1 if physical memory is swapped out to the disk, otherwise 0
    int used;               // 1 if current virtual page is occupied
    long int offset;        // offset bytes in disk
    physical_t *physical;   // map to a physical table entry
    size_t size;            // allocate memory size
} virtual_t;

static unsigned char pm_heap[PHYSICAL_SIZE];       // pre-allocated physcial memory
static virtual_t pm_virtual[VIRTUAL_BLOCK_NUM];    // virutal memory table
static physical_t pm_physical[PHYSICAL_BLOCK_NUM]; // physical memory table

static int physical_available = PHYSICAL_BLOCK_NUM;
static int virtual_available = VIRTUAL_BLOCK_NUM;

/*
Initialize physical memory table and virtual memory table.
Initially, each physical table entry points at the corresponding physical memory block;
each virtual table entry contains a physical table entry pointer that is set to NULL.
*/
void pm_init();

/*
Check if physical is exhausted.
*/
int is_physical_full();

/*
Check if virtual is exhausted.
*/
int is_virtual_full();

/*
Find first unused physical block.
*/
physical_t *find_first_available_physical_block();

/*
Find first used but not swapped virtual block.
*/
virtual_t *find_first_used_not_swapped_virtual_block();

/*
Swap out the first used but not swapped virtual block's physical block and return it.
*/
physical_t *swap_out();

/*
Helper function that map physical block and virtual block, if physical is not full, find the first unused
else swap out the first used and not swapped virtual block's physical and use it.(FIFO)
*/
void map_physical_virtual(virtual_t *virtual_block);

/*
Find the first available virtual block to assign, and map it with physical block accordingly.
*/
virtual_t *find_first_available_virtual_block();

/*
Malloc funtcion return a pointer to virtual block
*/
virtual_t *pm_malloc(size_t size);

/*
Swap in function, swap in a virtual block memory from file.
*/
void swap_in(virtual_t *virtual_block);

/*
Check if a virtual block is swapped or not, if swapped, swap in else do nothing, return the physcial address that can be used.
*/
void pm_check(virtual_t *virtual_block);

/*
Free used memory from file or physical.
*/
void pm_free(virtual_t *virtual_block);

/*
Write with thread safety
*/
void pm_write(virtual_t *virtual_block, char *string, int size);

/*
Read with thread safety
*/
char* pm_read(virtual_t *virtual_block);

/*
Return physical block number that is mapped to a virtual block
*/
int get_page_num(virtual_t *virtual_bock);


#endif