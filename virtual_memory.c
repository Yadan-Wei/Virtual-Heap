#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <pthread.h>

#include "virtual_memory.h"

void init_page_tables() {
    // initialize physical page table
    for (int i = 0; i < PPN; i++) {
        ptable[i].vpn = -1;
        ptable[i].isdirty = 0;
        ptable[i].isempty = 1;
    }
    // initialize virtual page table
    for (int j = 0; j < VPN; j++) {
        vtable[j].ppn = -1;
        vtable[j].ispresent = 0;
        char* paddr = NULL;
    }
}

int find_free_page() {
    // if virtual memory is full
    if (next_page == VPN) return -1;

    // if phys_mem is not full
    if (next_page < PPN) {
        // map vp to pp
        vtable[]
    }

    // else evict a page 
    // 1. save old page to disk 
    // 2. set the corresponding virtual page to invalid
    int vpn = next_page;
    save_page(queue_head);
    queue_head += (queue_head + 1) % PPN;

    vtable[vpn].ispresent = 0;

    return next_page;
    
}

void* vm_alloc() {
    // find a free page on physical memory
    int ppn = find_free_page();
    
    // if not found, swap a page from phys_mem to disk
    if (ppn == -1) {
        // evict a page and get a new ppn

    } 
    
}




int handle_page_fault(size_t ppn) {
    // Find the page to replace using FIFO head
    int evicted_page = queue_head;
    queue_head += (queue_head + 1) % PPN;

    // Save the physcial page to disk
    save_page(evicted_page);
    
    // update the virtual page who is not longer present
    int to_evict_vpn = ptable[evicted_page].vpn;
    // update virtual table 
    vtable[to_evict_vpn].ispresent = 0;
    vtable[to_evict_vpn].ppn = -1;
    vtable[to_evict_vpn].paddr = NULL;

    // If evicted page is for new memory allocation
    // return the free page
    if (ppn == -1) {
        return evicted_page;
    }
    // Else if the page is missing
    // Load the page from disk to phys_mem
    int to_load_vpn = ptable[ppn].vpn;
    load_page(to_load_vpn);
    vtable[to_load_vpn].ispresent = 1;
    vtable[to_load_vpn].paddr = ppn * PAGE_SIZE;
}

void save_page(size_t ppn) {
    // map the phys page to disk
    int vpn = ptable[ppn].vpn;
}

void load_page(size_t vpn) {
    // map the virtual page back to phys_mem
}