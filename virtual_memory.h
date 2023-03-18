#ifndef VIRTUAL_MEMORY.H
#define VIRTUAL_MEMORY.H

#define PAGE_SIZE 2048 // 4KB
#define PHY_MEM_SIZE 256 * PAGE_SIZE  // 1MB
#define VIRT_MEM_SIZE 2 * PHY_MEM_SIZE  //2MB
#define PPN (PHY_MEM_SIZE / PAGE_SIZE)   // physical page number
#define VPN (VIRT_MEM_SIZE / PAGE_SIZE)  // virtual page number
#define STORAGE_FILE "storage.bin"       // virtual memory stored in file

typedef struct page_table_entry pte_t;

static char phy_mem[PPN];   
static pte_t page_table[VPN];
static int queue[PPN];  // evict the physical page using fifo algo
static int queue_head = 0;   // for evicted page
static int queue_tail = 0;   // for allocated page
static int next_page = 0;


typedef struct phys_page {
    int vpn;       // virtual page number
    int isdirty;   // 1 if page is modifed, 0 othwise
    int isempty;   // 1 if page is avaialbe, 0 otherwise
} phys_page_t;

typedef struct virt_page {
    int ppn;       // physcial page number
    int ispresent; // 1 if page is present in physical memory, 0 othwise
    char* paddr;   // physical address
} virt_page_t;

static phys_page_t ptable[PPN];
static virt_page_t vtable[VPN];


/**
 * Initialize page table 
*/
void init_page_tables();

/**
 * Allocate virtual memory (only one page) for a process each time
 * Return pointer pointing at the physical address
*/
void* vm_alloc();

/**
 * Free a virutal page for a process 
*/
void vm_free(void *ptr);

/**
 * Find the first free physical page
*/
int find_free_page();
/**
 * Load a page from storage file to physical memory
*/
void load_page(size_t virt_page_num);

/**
 * Save a page from physical memory to the storage file
*/
void save_page(size_t phys_page_num);

/**
 * Handle page fault 
 * Return a physcial page number that is free
*/
int handle_page_fault(size_t phys_page_num);

/**
 * Map the virtual address to physical memory
 * Return the physical address in the physical memory
*/
char* vm_map(size_t vaddr);
/**
 * Check if a virtual page is missing before read/write operations
*/
int check_if_page_valid(size_t page_num);

#endif