

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "./bthread.h"

#define LOG(str) write(1, str, strlen(str))

void print(char *arg) {
    // both parent and child run this
    char buf[100];
    snprintf(&buf[0], 100, "Hello from pid %d, precious=%s\n", getpid(),
             arg);
    if (write(1, buf, strlen(buf)) < 0) {
      perror("write");
    }
}

#define ARR_SIZE 4096
bthread_mutex mtx;
int massive_array1[ARR_SIZE];
int protect_me = 12;
int massive_array2[ARR_SIZE];

void seed() {
    srand(time(NULL));
    for (int i = 0; i < ARR_SIZE; i++) {
        massive_array1[i] = rand() % ARR_SIZE;
        massive_array2[i] = rand() % ARR_SIZE;
    }
}

void slam_bus() {
    // Perform lots of writes and reads to memory
    // Try to avoid hitting caches.
    int idx = rand() % ARR_SIZE;
    for (int i = 0; i < 100; i++) {
        int *arr = i % 2 == 0 ? massive_array1 : massive_array2;
        // where should we jump to next?
        int next_idx = arr[idx];
        // next thread that comes here will go somewhere totally different
        arr[idx] = (ARR_SIZE - 1) - arr[idx];
        idx = next_idx;
    }
}

int thread_fun(void *arg) {
    for (int i = 0; i < 400000; i++) {
        bthread_mutex_lock(&mtx);
        assert(protect_me == 12);
        protect_me++;
        assert(protect_me == 13);
        protect_me--;
        assert(protect_me == 12);
        slam_bus();
        bthread_mutex_unlock(&mtx);
    }
    print((char *)arg);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s [threadname1] ... [threadnameN]\n", argv[0]);
        return 1;
    }
    seed();
    const int nthreads = argc - 1;

    printf("I am the parent, my pid is %d\n", getpid());

    for (int i = 0; i < nthreads; i++) {
        int s = bthread_create(&thread_fun, argv[i + 1]);
        if (s == -1) {
            fprintf(stderr, "thread creation failed\n");
            exit(1);
        }
    }
    
    bthread_collect();
}