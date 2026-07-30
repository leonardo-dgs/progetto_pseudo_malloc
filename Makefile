CC=gcc
CCOPTS=--std=gnu99 -O3
BINS=pseudo_malloc

all:	$(BINS)

%.o:	%.c
	$(CC) $(CCOPTS) -c -o $@  $<

pseudo_malloc:	main.c intmath.c bitmap_tree.c buddy_allocator.c mmap_allocator.c pseudo_malloc.c tests.c shell.c
	$(CC) $(CCOPTS) -o $@ $^

debug:
	make clean
	make CCOPTS="-Wall -Wextra -g"

clean:
	rm -rf *.o *~ $(BINS)
