#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

extern void fsroot(void);

int write_file(){
  char file_path[] = "/root/hi.txt";
  int writer_fd;
  writer_fd = open(file_path, O_WRONLY | O_CREAT);
  char buf[] = "hello friends\n";
  if(writer_fd < 0)
    {
      printf("File could not be opened for writing. fd is %d\n", writer_fd);
      return 1;
    }
  else
    {
      write(writer_fd, buf, sizeof(buf));
    }
  close(writer_fd);
  return sizeof(buf);
}

void read_file(int len){
  char file_path[] = "/root/hi.txt";
  int reader_fd;
  reader_fd = open(file_path, O_RDONLY );
  int content_len = 100;
  char content[content_len];
  memset(&content[0], 0, sizeof(content_len));
  if(reader_fd < 0)
    {
      printf("File could not be opened for reading.\n");
      return;
    }
  else
    {
      read(reader_fd, content, len);
    }
  printf("File holds: %s\n", content);
  

}

int print_files(void)
{
  printf("In print_files, about to open /root and list all files\n");
  DIR *d;
  struct dirent *dir;

  d = opendir("/root");
  if (d == NULL){
    printf("failed to open\n");
    return 0;
  }
  printf("Opendir successful\n");
  while ((dir = readdir(d)) != NULL) {
    printf("%s\n", dir->d_name);
  }
  closedir(d);
  printf("Done in print_files\n\n");
  return(0);
}

int main(int argc, char *argv[], char * envp[])
{
    int i;
    printf("about to call fsroot\n");
    fsroot();

    printf("argc: %d\n\n",argc);

    for(i=0; i<argc; i++)
        printf("argc[%d]=%s\n",i,argv[i]);

    printf("\nenvp: \n");
    for (i = 0; envp[i] != NULL; i++)
        printf("envp[%d]=%s\n",i, envp[i]);

	char cwd[PATH_MAX+1];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       printf("getcwd() error");
   }

  int len = write_file();
  read_file(len);

  print_files();

  printf("Done\n\n");

  return 0;
}
