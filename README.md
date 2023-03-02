# Virtual-Heap
```
typedef struct physical_t {
  int isUsed;
  size_t size;
  char* physcialAddr;
} physcial_t;

typedef struct virtual_t{
  int isSwapped;
  int isUsed;
  char* filePath;
  physical_t* physical;
  size_t size;
} virtual_t;
```
  
  
