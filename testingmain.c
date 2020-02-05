#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int printk(const char *format, ...);
extern int do_test(void);
typedef void (*smp_call_func_t)(void *info);
extern int smp_call_function_single(int cpuid, smp_call_func_t func, void *info,int wait);

void myprintk(void){
	printk("In function myprintk\n");
}

int testmain(void){

	/*
	printk("Starting  test\n");
	printk("\n ************* \n\n");
	if(do_test() == 0){
		printk("test passed\n");
	}else{
		printk("test failed\n");
	}
        printk("\n ************* \n\n");
	*/

	smp_call_function_single(7, myprintk, NULL, 1);

	return 0;
}

int kmain(void){

	/*
	pthread_attr_t attr;

	if (pthread_attr_init(&attr) != 0)
    	{
      		puts ("pthread_attr_init failed");
      		return 1;
    	}
               
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    	{
      		puts ("pthread_attr_setdetachstate failed");
      		return 1;
    	}
	
	pthread_t thk;
  	if (pthread_create (&thk, &attr, testmain, NULL) != 0)
    	{
      		puts ("testmain create failed");
      		return 1;
    	}
	*/
	
	pthread_t thk;
  	if (pthread_create (&thk, NULL, testmain, NULL) != 0)
    	{
      		puts ("testmain create failed");
      		return 1;
    	}
	

	return 0;
}
