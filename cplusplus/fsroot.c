#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sys/sysmacros.h>
#include <stdlib.h>
#include <dirent.h>

void fsroot(void){
	if (mknod ("/dev/root", S_IFBLK|0700, makedev (8,0)) == -1) {
		printk("mknod: /dev/root");
	} else {
		printk("mknod successful!\n");
	}

	if (mount ("/dev/root", "/root", "ext2", MS_NOATIME, "") == -1) {
		printk("mount: /root");
	} else {
		printk("mount successful!\n");
	}

        if (mknod ("/dev/urandom", S_IFCHR|0444, makedev (1,9)) == -1) {
                printk("mknod: /dev/urandom error");
        } else {
                printk("mknod /dev/urandom successful!\n");
        }
}
