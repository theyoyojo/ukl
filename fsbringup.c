#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdio.h>
#include <sys/sysmacros.h>
#include <stdlib.h>
#include <dirent.h>

extern int printk(const char *fmt, ...);

int fsbringup(void){

	// Building and Mounting tmpfs
  // Make a directory in existing fs
	if( mkdir ("/mytmpfs", 0777)== -1){
		printk("mkdir: /mytmpfs fail");
	} else {
		printk("mkdir /mytmpfs successful!\n");
	}

	if(mount("tmpfs", "/mytmpfs", "tmpfs", MS_MGC_VAL, "size=4g") == -1){
		printk("mount: /mytmpfs fail");
	} else {
		printk("mount /mytmpfs successful!\n");
	}

  // With just these 2, I see the dir made correctly
  // And the mount works.

	// Mounting root files system provided by QEMU

  // Make another directory
  // This fails for some reason. It also fails when you don't
  // make the mytmpfs dir.
	/* if( mkdir ("/root", 0777)== -1){ */
	/* 	printk("mkdir: /root fail"); */
	/* } else { */
	/* 	printk("mkdir /root successful!\n"); */
	/* } */

  // This works without the mkdir
	if (mknod ("/dev/root", S_IFBLK|0700, makedev (8,0)) == -1) {
		printk("mknod: /dev/root");
	} else {
		printk("mknod successful!\n");
	}
	
  // Source , target, type 
	if (mount ("/dev/root", "/root", "ext2", MS_NOATIME, "") == -1) {
		printk("mount: /root");
	} else {
		printk("mount successful!\n");
	}

  // Mount proc fs
	/* if( mkdir ("/run/mount", 0777)== -1){ */
	/* 	printk("mkdir: /run/mount fail"); */
	/* } else { */
	/* 	printk("mkdir /run/mount successful!\n"); */
	/* } */

  /* mount("none", "/proc", "proc", 0, NULL) */
    if(mount("none", "/proc", "proc", 0, NULL) == -1){
      printk("mount: /proc fail");
    } else {
      printk("mount /proc successful!\n");
    }


  return 0;
	// Copying files from /dev/root/ukl to tmpfs
	
	FILE *fptr;
	FILE *source, *target;
	char ch;

	// use appropriate location if you are using MacOS or Linux
	fptr = fopen("/mytmpfs/test.txt","w");
	
	if(fptr == NULL)
	{
		printk("Error while writing!\n");
		exit(1);
	}
	
	fprintf(fptr,"Testing the filesystem by writing to it\n");
	fclose(fptr);
	
	source = fopen("/mytmpfs/test.txt", "r");

	if( source == NULL )
	{
	   printk("source problem\n");
	   exit(EXIT_FAILURE);
	}

	target = fopen("/root/target.txt", "w");

	if( target == NULL )
	{
	   fclose(source);
	   printf("target problem\n");
	   exit(EXIT_FAILURE);
	}

	while( ( ch = fgetc(source) ) != EOF )
	   fputc(ch, target);

	printk("File copied successfully.\n");

	fclose(source);
	fclose(target);
	
	sync();
  /* while(1); */
	return 0;
}

