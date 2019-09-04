#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#define printff printk

int kmain(void) {
    void *b;
    char *p, *end;

    b = sbrk(0);
    printff("%lx\n", b);
    p = (char *)b;
    printff("%lx\n", p);
    end = p + 0x1000000;
    printff("%lx\n", end);
    brk(end);
    while (p < end) {
        *(p++) = 1;
    }
    brk(b);
    return 0;
}