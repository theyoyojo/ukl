#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#include <dirent.h>

#define main kmain
#define printf printk
int write_file(){
  char file_path[] = "/hi.txt";
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
  char file_path[] = "/hi.txt";
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
  printf("in main\n");
  DIR *d;
  struct dirent *dir;

  d = opendir("/myfile.txt");
  if (d == 0){
    printf("failed to open\n");
  }
  if (d) {
      while ((dir = readdir(d)) != NULL) {
          printf("%s\n", dir->d_name);
        }
      closedir(d);
    }
  printf("done main\n");
  return(0);
}

int main(int argc, char *argv[])
{
  int len = write_file();
  read_file(len);

  read_file(14);
  print_files();
  return 0;
}
