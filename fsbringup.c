#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sys/sysmacros.h>
#include <stdlib.h>

/*
extern int printk(const char *fmt, ...);
extern long ukl_open(const char* filename, int flags, unsigned short mode);
extern long ukl_close(int fd);
extern ssize_t ukl_write(int fd, const void* buf, size_t count);
extern int ukl_mount(char * dev_name, char * dir_name, char * type, unsigned long flags, void * data);
extern int ukl_chroot(const char * filename);
extern int ukl_chdir(const char * filename);
*/

int kmain(void){
	mkdir ("/sys", 0755);
	if (mknod ("/dev/root", S_IFBLK|0700, makedev (8,0)) == -1) {
		printk("mknod: /dev/root");
	} else {
		printk("mknod successful!\n");
	}

	if (ukl_mount ("/dev/root", "/root", "ext2", MS_NOATIME, "dax") == -1) {
		printk("mount: /root");
	} else {
		printk("mount successful!\n");
	}

	if (ukl_chroot ("/root") == -1) {
		printk ("chroot: /root");
	} else {
		printk("chroot successful!\n");
	}
	ukl_chdir ("/");

	/*
	int fp;

	fp = ukl_open("/home/test.txt", O_WRONLY | O_APPEND, S_IRWXU);
	ukl_write(fp, "This is testing for write...\n  ", 29);
	ukl_close(fp);
	*/

	FILE *fptr;
	
	// use appropriate location if you are using MacOS or Linux
	fptr = fopen("/home/test.txt","w");
	
	if(fptr == NULL)
	{
		printk("Error!");
		exit(1);
	}
	
	fprintf(fptr,"Testing the filesystem by writing to it\n");
	fclose(fptr);
	return 0;
}
