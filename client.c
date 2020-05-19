

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
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

bthread_mutex mtx;
int protect_me = 12;

int thread_fun(void *arg) {
    for (int i = 0; i < 400000; i++) {
        if (i % (400000 / 2) == 0) {
            printf("yay, %s\n", (char *)arg);
        }
        bthread_mutex_lock(&mtx);
        assert(protect_me == 12);
        protect_me++;
        protect_me--;
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