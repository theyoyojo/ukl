#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

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

int main(int argc, char *argv[])
{

  int len = write_file();
  read_file(len);
  return 0;
}
