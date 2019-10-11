#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int printk(const char *format, ...);
extern int do_test1(void);
extern int do_test2(void);
extern int do_test3(void);
extern int do_test4(void);
extern int do_test5(void);
extern int do_test6(void);
extern int do_test7(void);
extern int do_test8(void);
extern int do_test9(void);
extern int do_test10(void);
extern int do_test11(void);
extern int do_test12(void);
extern int do_test13(void);
extern int do_test14(void);
extern int do_test15(void);
extern int do_test16(void);
extern int do_test17(void);
extern int do_test18(void);
extern int do_test19(void);
extern int do_test20(void);
extern int do_test21(void);
extern int do_test22(void);
extern int do_test23(void);
extern int do_test24(void);
extern int do_test25(void);

int testmain(void){
	printk("Starting  tests\n");
	/*printk("\n ************* \n\n");
	if(do_test1() == 0){
		printk("tst-1 passed\n");
	}else{
		printk("tst-1 failed\n");
	}*/
        printk("\n ************* \n\n");
	if(do_test2() == 0){
		printk("tst-2 passed\n");
	}else{
		printk("tst-2 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test3() == 0){
		printk("tst-3 passed\n");
	}else{
		printk("tst-3 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test4() == 0){
		printk("tst-4 passed\n");
	}else{
		printk("tst-4 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test5() == 0){
		printk("tst-5 passed\n");
	}else{
		printk("tst-5 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test6() == 0){
		printk("tst-6 passed\n");
	}else{
		printk("tst-6 failed\n");
	}
        /*printk("\n ************* \n\n");
	if(do_test7() == 0){
		printk("tst-7 passed\n");
	}else{
		printk("tst-7 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test8() == 0){
		printk("tst-8 passed\n");
	}else{
		printk("tst-8 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test9() == 0){
		printk("tst-9 passed\n");
	}else{
		printk("tst-9 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test10() == 0){
		printk("tst-10 passed\n");
	}else{
		printk("tst-10 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test11() == 0){
		printk("tst-11 passed\n");
	}else{
		printk("tst-11 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test12() == 0){
		printk("tst-12 passed\n");
	}else{
		printk("tst-12 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test13() == 0){
		printk("tst-13 passed\n");
	}else{
		printk("tst-13 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test14() == 0){
		printk("tst-14 passed\n");
	}else{
		printk("tst-14 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test15() == 0){
		printk("tst-15 passed\n");
	}else{
		printk("tst-15 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test16() == 0){
		printk("tst-16 passed\n");
	}else{
		printk("tst-16 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test17() == 0){
		printk("tst-17 passed\n");
	}else{
		printk("tst-17 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test18() == 0){
		printk("tst-18 passed\n");
	}else{
		printk("tst-18 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test19() == 0){
		printk("tst-19 passed\n");
	}else{
		printk("tst-19 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test20() == 0){
		printk("tst-20 passed\n");
	}else{
		printk("tst-20 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test21() == 0){
		printk("tst-21 passed\n");
	}else{
		printk("tst-21 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test22() == 0){
		printk("tst-22 passed\n");
	}else{
		printk("tst-22 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test23() == 0){
		printk("tst-23 passed\n");
	}else{
		printk("tst-23 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test24() == 0){
		printk("tst-24 passed\n");
	}else{
		printk("tst-24 failed\n");
	}
        printk("\n ************* \n\n");
	if(do_test25() == 0){
		printk("tst-25 passed\n");
	}else{
		printk("tst-25 failed\n");
	}
	if(tsttstexcept() == 0){
                printk("tst-25 passed\n");
        }else{
                printk("tst-25 failed\n");
        }
	*/
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
