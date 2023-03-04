# Virtual-Heap
```
#define PHYSICAL_SIZE 10 * 1024 * 1024
#define VIRTUAL_SIZE 2 * PHYSICAL_SZIE
#define VIRTUAL_BLOCK_NUM VIRTUAL_SIZE / (4 * 1024)
#define PHYSICAL_BLOCK_NUM PHYSCIAL_SIZE / (4 * 1024)

typedef struct physical_t {
  int used;
  size_t size;
  char* physcial_addr;
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


int is_physical_full() {
  return 1 if physical_available is 0 else 0
}

int is_virtual_full() {
  return 1 if virtual_available is 0 else 0
}

virtual_t* pm_malloc(size_t size) {
  if is_virtual_full() return NULL
  virtual_t virtual_block = find_first_available_virtual_block();
  virtual_block->used = 1;
  virtual_available -= 1;
  return &virtual_block  
}


virtual_t find_first_available_virtual_block() {
  for(int i = 0; i < VIRTUAL_BLOCK_NUM; i++) {
    virtual_t virtual_block = pm_virtual[i];
    if not virtual_block->used {
      map_physical_virtual(virtual_block);
      return virtual_block;
    }
  }
}

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

physcial_t find_first_avaiable_physical_block() {
  for(int i = 0; i < PHYSICAL_BLOCK_NUM; i++) {
    physical_t physical_block = pm_physical[i];
    if not physical_block->used {
      return physical_block;
    }
  }
}

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

virtual_t find_first_used_not_swapped_virtual_block() {
  for(int i = 0; i < VIRTUAL_BLOCK_NUM; i++) {
    virtual_t virtual_block = pm_virtual[i];
    if virtual_block->used and not virtual_block->swapped{
      return virtual_block;
    }
  }
}

void * pm_check(virtual_t * virtual_block) {
  if virtual_block-> swapped 
      swap_in(virtual_block)
  return virtual_block-> physical ->physcial_addr    
}

void swap_in(virtual_block) {
  virtual_block-> physical = swap_out();
  fopen vritual_block->filepath
  fread...
  write to physical_block-> physical_addr
  fclose
  
  virtual_block->swapped = 0
  virtual_block->file_path = NULL
    
}






```
  
  
