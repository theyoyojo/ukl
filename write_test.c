#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int printk(const char *format, ...);

#define PAGE_SIZE 4096
//file_size = PAGE_SIZE * 10000;
file_size = PAGE_SIZE * 10000;

void write_test(struct timespec *diffTime) {
        char *buf;
        int ii = 0;
	for(ii = 0; ii < 2; ii++){
		buf = (char *) malloc (sizeof(char) * file_size);
        	printk("write_test 1: chunk = 0x%08lx\n", *(buf-0x8));
        	printk("buf = 0x%lx\n", buf);
        	for (int i = 0; i < file_size; i++) {
			//if(ii == 1 && i%1000 == 0){
			//	printk("write_test 1.%d: chunk = 0x%lx\n", i, *(buf-0x8));
			//}
                	buf[i] = 'a';
        	}
        	printk("write_test 2: chunk = 0x%08lx\n", *(buf-0x8));
        	free(buf);
	}
        return;
}

int kmain(void){
        pthread_t thk;
        if (pthread_create (&thk, NULL, write_test, NULL) != 0)
        {
                printf("testmain create failed");
                return 1;
        }

        return 0;
}

