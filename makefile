
.PHONY: all clean

all: client bench

client: client.c bthread.c
	gcc -O0 -Wall -Wextra -std=gnu99  $^ -o $@

bench: benchmark.c bthread.c
	gcc -O0 -Wall -Wextra -std=gnu99  $^ -o $@

clean:
	rm -f bench client