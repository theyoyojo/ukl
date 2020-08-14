#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#define __FUNC__ __FUNCTION__
unsigned int page_size = 1<<12;

extern int cmd_record(int argc, const char **target_argv);
extern int fsbringup(void);

const char *cmd_record_argv[4] = { "record", "-ag", 0};

void benchmark(){
  /* printf("Top of %s\n", __FUNC__); */
  /* int i = 1 << 28; */
  /* while(i--); */
  while(1);
}

void* start_perf(void *ptr){
  printf("Top of %s\n", __FUNC__);
  int cmd_record_argc = 2;
  cmd_record(cmd_record_argc, cmd_record_argv);
}

void launch_perf_pthread(pthread_t * tid){

  void *vp = NULL;

  printf("Top of %s\n", __FUNC__);
  int result;
  /* HACK Pretending this is up immediately */
  if ((result = pthread_create(tid, NULL, start_perf, vp)) != 0) {
    printf("Error creating thread\n");
  }
}

void cancel_perf_pthread(pthread_t * tid){
  printf("Top of %s\n", __FUNC__);
  int result;
  if ((result = pthread_kill(*tid, SIGINT)) != 0) {
    printf("Error canceling thread\n");
    /* Handle Error */
  }
}

int main()
{
  pthread_t tid;
  pthread_t *tid_ptr = &tid;

  fsbringup();

  printf("Main th %lx\n", pthread_self());

  launch_perf_pthread(tid_ptr);

  // Ignoring how long it takes for perf to come up.
  benchmark();

  cancel_perf_pthread(tid_ptr);

  printf("about to join th %lx\n", pthread_self());

  pthread_join(tid, NULL);

  printf("All done\n");
  while(1);
  return 0;
}
