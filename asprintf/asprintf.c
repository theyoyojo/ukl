#define _GNU_SOURCE
#include <stdio.h>

int main(){
char *my_ptr=NULL;
 printk("in main!\n");

 asprintf(&my_ptr, "cycles%s%s%s", "foo", "bar", "baz");

 printk("the string is %s\n", my_ptr);
}
