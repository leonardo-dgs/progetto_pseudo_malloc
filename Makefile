CC=gcc
CCOPTS=--std=gnu99 -Wall -O3
BINS=pseudo_malloc

all:	$(BINS)

%.o:	%.c
	$(CC) $(CCOPTS) -c -o $@  $<

fat:	main.c fat.c
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(BINS)
