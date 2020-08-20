#define _GNU_SOURCE
#include <stdio.h>

int main(){
char *my_ptr=NULL;
 /* printk("in main!\n"); */

 printk("in main\n");

 // look at init of vtables
 /* asprintf(&my_ptr, "cycles%s%s%s%s", "foo", "bar", "baz", "boz"); */
 /* asprintf(&my_ptr, "cycles%d", 42); */

 // Produce assembly code, what is symbol? Do on both systems.
 asprintf(&my_ptr, "str");

 printk("the string is %s\n", my_ptr);
}
