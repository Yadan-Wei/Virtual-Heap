CC=clang
CFLAGS=-g -Wall -std=c99 
LIBS=-lpthread
DEPS=virtualHeap.h
OBJ=virtualHeap.o vm_test.o

# $@: refer to the left side of :
# $<: refer to the first item in the dependencies list
# $^: refer to the right side of :
## -c: to generate the object files

all: vmtest

vmtest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

%.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o vmtest