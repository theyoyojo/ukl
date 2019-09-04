#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h> 

extern int printk(const char *fmt, ...);
extern long stack_dec_populate(int i);
extern long stack_dec_print(void);


#define QUEUE_SIZE 1

void *stackworker(void);
void *worker(void);

int kmain(int argc, char *argv[]){

	pthread_t thread_id[QUEUE_SIZE];
	int i = 0;


	for(i = 0; i < QUEUE_SIZE; i++){
		if (pthread_create( &thread_id[i] , NULL , worker , NULL) < 0) {
			printk("could not create thread");
			return 1;
		}	
	}

	for(i = 0; i < QUEUE_SIZE; i++){
		if (pthread_join(thread_id[i], NULL) < 0) {
			printk("could not join thread");
			return 1;
		}else{
			printk("pthread_join worked for thread id %d\n",i);
		}
	}
	
	return 0;
}

void *stackworker(void)
{
	long addr;
	int i = 0;
	addr = stack_dec_populate(0x10);
	printk("Start stack address = %lx\n", addr+0x10);
	for(i = 1; i <= 10; i++){
		addr = stack_dec_populate(i*2048);
	}
	printk("End stack address = %lx\n", addr);
    return 0;
} 

void *worker(void)
{
	long addr;
	int i = 0;
	for(i = 1; i <= 100; i++){
		addr = stack_dec_print();
		printk("%d. Stack address = %lx\n", i, addr);
	}
    return 0;
} 