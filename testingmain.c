#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int printk(const char *format, ...);
extern int do_test(void);

int testmain(void){
	printk("Starting  test\n");
	printk("\n ************* \n\n");
	if(do_test() == 0){
		printk("test passed\n");
	}else{
		printk("test failed\n");
	}
        printk("\n ************* \n\n");
}

int kmain(void){
	pthread_t thk;
  	if (pthread_create (&thk, NULL, testmain, NULL) != 0)
    	{
      		puts ("testmain create failed");
      		return 1;
    	}
	return 0;
}
