CC = gcc
OPT = -O2 -Wall -Wextra
INC = -I..
LIB = -lpthread
EXAMPLES = ringbuffer_status fast_producer concurrency atomic_int

all: $(EXAMPLES)

%: %.c
	$(CC) $(OPT) $(INC) -o $@ $< ../spsc_ringbuff.c $(LIB)

clean:
	rm -f $(EXAMPLES)

