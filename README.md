# Virtual-Heap

## Design for Virtual Memory Allocation on Static Memory 
In this program we designed a simple version of virtual heap. We allocate the static memory as phy_memo[page_numbers] where page_numbers is the number of physical page. The program should also provide thread-safe processing on the shared physical memory. 

## Data Structure

### Pages:
A chunk of memory with a size of 4KB (4096). Addresses in the same virtual page get mapped to address in the same physical page. The page table determines the mapping.
Page size: 4KB (2^12 Bytes)\
Physical memory size: 1MB (2^20 Bytes)\
Physical page number: 256 (1MB/4KB)\
Virtual page number: 2*256

### Virtual Address(VA): 
A contiguous memory what the program uses.
Virtual Page Number(VPN): index of Page Table Entry

### Physical Address(PA) 
It actually determines where in memory to go in the static memory. 
Physical Page Number(PPN): index of phy_memo[page_numbers]

### Page Table Entry(PTE):
Page table helps translate the virtual address to the physcial address. Page table is an array with size of VPN. Each stroed row of the page table is called a page table entry. 

Page Table:\
------------------------------------------------------------------\
Index=VPN | Page valid | Page dirty | Read/Write permission | PPN \
------------------------------------------------------------------\
0         |            |            |                       |     \
1         |            |            |                       |     \
...       |            |            |                       |     \
(Max VPN) |            |            |                       |     \
------------------------------------------------------------------\

## Algorithm 
Page Replacement algorithm: FIFO
How to handle page fault:

  
  
