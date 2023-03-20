# Virtual-Heap

## Introduction
This is a CS5600 practicum. In this program we try to design a simple version of virtual heap on static memory and provide safe threaded processing on the shared virtual heap. 

Author: 
* Yadan Wei @Yadan-Wei
* Qiaojun Chen @qiaojunch 


## Data Structure

* Physical memory: 

`static unsigned char pm_heap[PHYSICAL_SIZE];`

Preallocated static memory with total size of 20 KB (for test use, can be changed in .h file).

Page size: 4096 bytes = 4 KB

Number of pages: 5

PM size: 20 KB

* Physical block: 

a structure that points to the allocated real physical address and record its usage status

```
typedef struct physical_t
{
    int used;
    size_t size;
    void *physical_addr; // map to physical memory address
} physical_t;
```

* Virtual memory: 

the memory can be allocated, currently 2 times of real physical memory

Page size: 4 KB

Number of page: 2 * (physical memory size)

VM size: 40 KB

* Virtual block: 

A structure that records the allocated information and status
```
typedef struct virtual_t
{
    int swapped;          // 1 if physical memory is swapped out to the disk, otherwise 0
    int used;             // 1 if current virtual page is occupied
    long int offset;      // offset bytes in disk
    physical_t *physical; // map to a physical table entry
    size_t size;          // allocate memory size
} virtual_t;
```

* Virtual block and physical block data table:

Use array to store all physical blocks and virtual blocks.

```
static virtual_t pm_virtual[VIRTUAL_BLOCK_NUM];    // virutal memory table
static physical_t pm_physical[PHYSICAL_BLOCK_NUM]; // physical memory table
```

* Backing store bin: 

A file named memory.dat.

When physical memory is not enough, some physical memory will be swapped out and saved into this file.

* Relationship

![未命名文件](https://user-images.githubusercontent.com/84998528/226260733-398f86fe-afde-429f-9051-fdb5a3b4e839.jpg)



## Algorithm 

### Malloc memory:

Use `virtual_t *pm_malloc(size_t size);` function to malloc virtual memory and return a pointer to assigned virtual block.

  - Virtual memory is full:
  
    return NULL.
  - Physical memory is enough:
  
    Find the first available physical block and mapped to the first available virtual block.
  - Physical memory is not enough:
  
    Find first used and unswapped virtual block and swapped out its physical memory to file and remapped to the first unused virtual block and return.
    
 ### Check memory is swapped or not
 
 Use `void pm_check(virtual_t *virtual_block);` to check if this memory is swapped out.
 
  - Swapped:
  
    - Physcial memory is full
    
      Find the first unswapped and used virtual block, swapped out its physical block and assign it to the checked virtual block and read its memory back from file.
    
    - Physical memory is not full
    
      Find the first unused physical block and assign it tp checked virtual block and read its memory back from file.
    
   - Not swapped
   
      Do nothing.
    
 ### Write to memory:
 
 
 Use `void pm_write(virtual_t *virtual_block, char *string, int size);` function to write to allocated memory.
 
 
 Within this function, call `void pm_check(virtual_t *virtual_block);` before writing to make sure the physical address is available.
 
 ### Read from memory:
 
 Use `char *pm_read(virtual_t *virtual_block);` function to read from allocated memory.
 
 Within this function, call `void pm_check(virtual_t *virtual_block);` before reading make sure the physical address is available for reading.
 
 ### Free memory:
 
  - Memory is swapped out
 
    Just free the status of virtual block and increase the number of unused virtual block.
 
  - Memory is not swapped out
 
    Free both virtual block and physical block and update their status.
 
 ### Thread safety
 
 Use pthread_mutex_t to make sure the program is thread safe.
 
 
 ### Test
 
 Test cased covered single memory allocate, free and multi-thread memory allocate, free and the ability to swap out when physical memory is full.
 
 Test also covers write to and read from memory to make sure the memory is accessiable in swapped out situation.
 
 ```
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

```

## Run the program

Put all file into a folder(including memory.dat), make in the terminal and check ./vmtest will see the test result of all test cases.

To change the size of virtual memory and physical memory, update the variable value defined in the header file.
 
 
 
 
 
 
 
  
  
