// Basic userspace handshake using futexes, for two processes.
//
// Eli Bendersky [http://eli.thegreenplace.net]
// This code is in the public domain.
#include <errno.h>
#include <linux/futex.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h> 

// The C runtime doesn't provide a wrapper for the futex(2) syscall, so we roll
// our own.
int futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
          int* uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

// Waits for the futex at futex_addr to have the value val, ignoring spurious
// wakeups. This function only returns when the condition is fulfilled; the only
// other way out is aborting with an error.
void wait_on_futex_value(int* futex_addr, int val) {
  while (1) {
    int futex_rc = futex(futex_addr, FUTEX_WAIT, val, NULL, NULL, 0);
    printf("In wait_on_futex_value.\t futex_rc = %d\n", futex_rc);
    if (futex_rc == -1) {
      if (errno != EAGAIN) {
        perror("futex");
        exit(1);
      }
    } else if (futex_rc == 0) {
      if (*futex_addr == val) {
        // This is a real wakeup.
        return;
      }
    } else {
      abort();
    }
  }
}

// A blocking wrapper for waking a futex. Only returns when a waiter has been
// woken up.
void wake_futex_blocking(int* futex_addr) {
  while (1) {
    int futex_rc = futex(futex_addr, FUTEX_WAKE, 1, NULL, NULL, 0);
    printf("In wake_futex_blocking.\t futex_rc = %d\n", futex_rc);
    if (futex_rc == -1) {
      perror("futex wake");
      exit(1);
    } else if (futex_rc > 0) {
      return;
    }
  }
}

void child_func(int* shared_data){
    // Child process
    printf("child waiting for A\n");
    wait_on_futex_value(shared_data, 0xA);

    printf("child writing B\n");
    // Write 0xB to the shared data and wake up parent.
    *shared_data = 0xB;
    wake_futex_blocking(shared_data);

    return;
}

void parent_func(int* shared_data){
  pthread_t thread_id;
  if (pthread_create(&thread_id, NULL , &child_func , shared_data) != 0) {
    perror("fork");
    exit(1);
  }

    // Parent process.
    printf("parent writing A\n");
    // Write 0xA to the shared data and wake up child.
    *shared_data = 0xA;
    wake_futex_blocking(shared_data);

    printf("parent waiting for B\n");
    wait_on_futex_value(shared_data, 0xB);

    // Wait for the child to terminate.
    if(pthread_join(thread_id, NULL) < 0) {
      printf("could not join thread");
      return 1;
    }
}

int main(int argc, char** argv) {
  pthread_t thread_id;
  
  int shared_data;// = shmat(shm_id, NULL, 0);
  shared_data = 0;

  if (pthread_create(&thread_id, NULL , &parent_func , &shared_data) != 0) {
    perror("fork");
    exit(1);
  }

    // Wait for the child to terminate.
    if(pthread_join(thread_id, NULL) < 0) {
      printf("could not join thread");
      return 1;
    }

  return 0;
}