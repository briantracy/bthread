

CFLAGS := -O0 -Wall -Wextra -std=gnu99 -fsanitize=address

client: client.c bthread.c
	gcc $(CLFAGS) $^ -o $@