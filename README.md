# Virtual-Heap
```
#define PHYSICAL_SIZE 10 * 1024 * 1024
#define VIRTUAL_SIZE 2 * PHYSICAL_SZIE
#define PAGE_SIZE 4 * 1024
#define VIRTUAL_BLOCK_NUM VIRTUAL_SIZE / PAGE_SIZE
#define PHYSICAL_BLOCK_NUM PHYSCIAL_SIZE / PAGE_SIZE

typedef struct physical_t {
  int used;
  size_t size;
  char* physical_addr;
} physcial_t;

typedef struct virtual_t{
  int swapped;
  int used;
  char* file_path;
  physical_t physical;
  size_t size;
} virtual_t;

static unsigned char pm_heap[PHYSICAL_SIZE];
static physical_t pm_virtual[VIRTUAl_BLOCK_NUM];
static virtual_t pm_physical[PHYSICAL_BLOCK_NUM];

int physical_available;
int virtual_available;

/*
init all physical blocks point to physical address
/*
void pm_init() {

  for(int i = 0; i<PHYSICAL_BLOCK_NUM; i++) {
      physical_t physical_block;
      physical_block->size = PAGE_SIZE
      physical_block->used = 0
      physcial_block->physical_addr = pm_heap[i*PAGE_SIZE]
      pm_physical[i] = physcial_block
  }

}

/*
Check if physical is exhausted.
*/
int is_physical_full() {
  return 1 if physical_available is 0 else 0
}

/*
Check if virtual is exhausted.
*/
int is_virtual_full() {
  return 1 if virtual_available is 0 else 0
}

/*
Malloc funtcion return a pointer to virtual block
*/
virtual_t* pm_malloc(size_t size) {
  if is_virtual_full() return NULL
  virtual_t virtual_block = find_first_available_virtual_block();
  virtual_block->used = 1;
  virtual_available -= 1;
  return &virtual_block  
}

/*
Find the first available virtual block to assign, and map it with physical block accordingly.
*/
virtual_t find_first_available_virtual_block() {
  for(int i = 0; i < VIRTUAL_BLOCK_NUM; i++) {
    virtual_t virtual_block = pm_virtual[i];
    if not virtual_block->used {
      map_physical_virtual(virtual_block);
      return virtual_block;
    }
  }
}

/*
Helper function that map physical block and virtual block, if physical is not full, find the first unused
else swap out the first used and not swapped virtual block's physical and use it.(FIFO)
*/
void map_physical_virtual(virtual_block) {
  physical_t physical_block;
  if is_physical_full() {
    physical_block = swap_out()
  } else {
    physical_block = find_first_available_physical_block();
    physical_block->used = 1;
    physical_available -= 1;
  }
  virtual_block->physical = physical_block;
}

/*
Find first unused physical block.
*/
physcial_t find_first_avaiable_physical_block() {
  for(int i = 0; i < PHYSICAL_BLOCK_NUM; i++) {
    physical_t physical_block = pm_physical[i];
    if not physical_block->used {
      return physical_block;
    }
  }
}

/*
Swap out the first used but not swapped virtual block's physical block and return it.
*/
physical_t swap_out() {
  virtual_t swap_out_block = find_first_used_not_swapped_virtual_block();
  physical_t swap_out_physical_block = swap_out_block->physical;
  
  fopen file....
  
  for(int i = 0; i < swap_out_physical_block->size; i++) {
    fwrite ...
  }
  fclose ...
  swap_out_block-> file_path = file
  swap_out_block->physcial = NULL
  swap_out_block->swapped = 1
  return swap_out_physical_block
}

/*
Find first used but not swapped virtual block.
*/
virtual_t find_first_used_not_swapped_virtual_block() {
  for(int i = 0; i < VIRTUAL_BLOCK_NUM; i++) {
    virtual_t virtual_block = pm_virtual[i];
    if virtual_block->used and not virtual_block->swapped{
      return virtual_block;
    }
  }
}

/*
Check if a virtual block is swapped or not, if swapped, swap in else do nothing, return the physcial address that can be used.
*/
void * pm_check(virtual_t * virtual_block) {
  if virtual_block-> swapped 
      swap_in(virtual_block)
  return virtual_block-> physical->physcial_addr    
}

/*
Swap in function, swap in a virtual block memory from file.
*/
void swap_in(virtual_block) {
  virtual_block-> physical = swap_out();
  fopen vritual_block->filepath
  fread...
  
  write to physical_block-> physical_addr
  file delete?...
  fclose ...
  
  virtual_block->swapped = 0
  virtual_block->file_path = NULL   
}


/*
Free used memory from file or physical.
*/
void pm_free(virtual_block) {
  
  if virtual_block->swapped {
    do nothing? or delete file
    virtual_block->swapped = 0
    virtual_block->file_path = NULL
  } else {
    virtual_block-> physical -> used = 0
    virtual_block-> physical = NULL
    physical_available += 1
  }
  virtual_block->used = 0;
  virtual_available += 1 
}






```
  
  
