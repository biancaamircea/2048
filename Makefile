# compiler setup
CC=gcc
CFLAGS=-Wall -g -lncurses -lm 

# define targets
TARGETS = 2048

build: $(TARGETS)

2048: 2048.c
	$(CC) $(CFLAGS) 2048.c -lm -o 2048

run: build
	./2048

clean:
	rm -f $(TARGETS)

.PHONY: clean
