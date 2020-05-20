

CFLAGS = -O0 -Wall -Wextra -std=gnu99 -fsanitize=address -pg

client: client.c bthread.c
	gcc -O0 -Wall -Wextra -std=gnu99  $^ -o $@ -g -pg

bench: benchmark.c bthread.c
	gcc -O0 -Wall -Wextra -std=gnu99  $^ -o $@