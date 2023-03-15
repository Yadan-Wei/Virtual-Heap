#ifndef VIRTUAL_MEMORY.H
#define VIRTUAL_MEMORY.H

#define PHY_MEM_SIZE 256 * 2098  // 1MB
#define VIRT_MEM_SIZE 2 * PHY_MEM_SIZE  //2MB
#define PAGE_SIZE 2098 // 4KB
#define PPN (PHY_MEM_SIZE / PAGE_SIZE)   // physical page number
#define VPN (VIRT_MEM_SIZE / PAGE_SIZE)  // virtual page number
#define STORAGE_FILE "storage.bin"       // virtual memory stored in file

typedef struct page_table_entry pte_t;

static char phy_mem[PPN];   
static pte_t page_table[VPN];
static int fifo_queue[PPN];  // evict the physical page using fifo algo
static int queue_head = 0;
static int queue_tail = 0;

struct page_table_entry {
    size_t virt_page_num;  // virtual page number = index of page table
    size_t phy_page_num;   // physical page number
    bool page_valid;    // check if page is stored in phy_mem
    bool page_dirty;    // check if page is modified in phy_mem
    bool is_write;      // check for write permission
    char *data;
};

/**
 * Initialize page table 
*/
void init_page_table();

/**
 * Allocate virtual memory (only one page) for a process each time
 * Return a pointer pointing at the page table entry
*/
void* vm_alloc();

/**
 * Free a virutal page for a process 
*/
void vm_free(void *ptr);


/**
 * Load a page from storage file to physical memory
*/
void load_page(size_t virt_page_num);

/**
 * Save a page from physical memory to the storage file
*/
void save_page(size_t virt_page_num);

/**
 * Handle page fault 
*/
void handle_page_fault(size_t virt_page_num);

/**
 * Check if a virtual page is missing before read/write operations
*/
int check_if_page_valid(size_t virt_page_num);

#endif