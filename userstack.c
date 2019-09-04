#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h> 

extern long stack_dec_print(void);

#define QUEUE_SIZE 1

void *worker(void);

int main(int argc, char *argv[]){

	pthread_t thread_id[QUEUE_SIZE];
	int i = 0;


	for(i = 0; i < QUEUE_SIZE; i++){
		if (pthread_create( &thread_id[i] , NULL , worker , NULL) < 0) {
			printf("could not create thread");
			return 1;
		}	
	}

	for(i = 0; i < QUEUE_SIZE; i++){
		if (pthread_join(thread_id[i], NULL) < 0) {
			printf("could not join thread");
			return 1;
		}else{
			printf("pthread_join worked for thread id %d\n",i);
		}
	}
	
	return 0;
}

void *worker(void)
{
	long addr;
	int i = 0;
	for(i = 0; i < 100; i++){
		addr = stack_dec_print();
		printf("%d. Stack address = %lx\n", i, addr);
	}
    return 0;
} 