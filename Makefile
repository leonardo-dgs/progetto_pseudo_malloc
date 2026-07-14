CC=gcc
CCOPTS=--std=gnu99 -Wall -O3
BINS=pseudo_malloc

all:	$(BINS)

%.o:	%.c
	$(CC) $(CCOPTS) -c -o $@  $<

pseudo_malloc:	main.c bitmap_tree.c buddy_allocator.c mmap_allocator.c pseudo_malloc.c tests.c
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(BINS)
