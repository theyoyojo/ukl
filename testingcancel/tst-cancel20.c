/* Copyright (C) 2003-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define printf printk
#define puts printk


static int fd20[4];
static pthread_barrier_t b20;
volatile int in_sh_body20;
unsigned long cleanups20;

static void
cl20 (void *arg)
{
  cleanups20 = (cleanups20 << 4) | (long) arg;
}


static void __attribute__((noinline))
sh_body20 (void)
{
  char c;

  pthread_cleanup_push (cl20, (void *) 1L);

  in_sh_body20 = 1;
  if (read (fd20[2], &c, 1) == 1)
    {
      puts ("read succeeded");
      exit (1);
    }

  pthread_cleanup_pop (0);
}


static void
sh20 (int sig)
{
  pthread_cleanup_push (cl20, (void *) 2L);
  sh_body20 ();
  in_sh_body20 = 0;

  pthread_cleanup_pop (0);
}


static void __attribute__((noinline))
tf_body20 (void)
{
  char c;

  pthread_cleanup_push (cl20, (void *) 3L);

  int r = pthread_barrier_wait (&b20);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("child thread: barrier_wait failed");
      exit (1);
    }

  if (read (fd20[0], &c, 1) == 1)
    {
      puts ("read succeeded");
      exit (1);
    }

  read (fd20[0], &c, 1);

  pthread_cleanup_pop (0);
}


static void *
tf20 (void *arg)
{
  pthread_cleanup_push (cl20, (void *) 4L);
  tf_body20 ();
  pthread_cleanup_pop (0);
  return NULL;
}


static int
do_one_test20 (void)
{
  in_sh_body20 = 0;
  cleanups20 = 0;
  if (pipe (fd20) != 0 || pipe (fd20 + 2) != 0)
    {
      puts ("pipe failed");
      return 1;
    }

  pthread_t th;
  if (pthread_create (&th, NULL, tf20, NULL) != 0)
    {
      puts ("create failed");
      return 1;
    }

  int r = pthread_barrier_wait (&b20);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("parent thread: barrier_wait failed");
      return 1;
    }

  sleep (1);

  r = pthread_kill (th, SIGHUP);
  if (r)
    {
      errno = r;
      printf ("pthread_kill failed %m\n");
      return 1;
    }

  while (in_sh_body20 == 0)
    sleep (1);

  if (pthread_cancel (th) != 0)
    {
      puts ("cancel failed");
      return 1;
    }

  void *ret;
  if (pthread_join (th, &ret) != 0)
    {
      puts ("join failed");
      return 1;
    }

  if (ret != PTHREAD_CANCELED)
    {
      puts ("result is wrong");
      return 1;
    }

  if (cleanups20 != 0x1234L)
    {
      printf ("called cleanups20 %lx\n", cleanups20);
      return 1;
    }

  /* The pipe closing must be issued after the cancellation handling to avoid
     a race condition where the cancellation runs after both pipe ends are
     closed.  In this case the read syscall returns EOF and the cancellation
     must not act.  */
  close (fd20[0]);
  close (fd20[1]);
  close (fd20[2]);
  close (fd20[3]);

  return 0;
}


int
do_test20 (void)
{
  stack_t ss;
  ss.ss_sp = malloc (2 * SIGSTKSZ);
  if (ss.ss_sp == NULL)
    {
      puts ("failed to allocate alternate stack");
      return 1;
    }
  ss.ss_flags = 0;
  ss.ss_size = 2 * SIGSTKSZ;
  if (sigaltstack (&ss, NULL) < 0)
    {
      printf ("sigaltstack failed %m\n");
      return 1;
    }

  if (pthread_barrier_init (&b20, NULL, 2) != 0)
    {
      puts ("barrier_init failed");
      return 1;
    }

  struct sigaction sa;
  sa.sa_handler = sh20;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction (SIGHUP, &sa, NULL) != 0)
    {
      puts ("sigaction failed");
      return 1;
    }

  puts ("sa_flags = 0 test");
  if (do_one_test20 ())
    return 1;

  sa.sa_handler = sh20;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_ONSTACK;

  if (sigaction (SIGHUP, &sa, NULL) != 0)
    {
      puts ("sigaction failed");
      return 1;
    }

  puts ("sa_flags = SA_ONSTACK test");
  if (do_one_test20 ())
    return 1;

#ifdef SA_SIGINFO
  sa.sa_sigaction = (void (*)(int, siginfo_t *, void *)) sh20;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;

  if (sigaction (SIGHUP, &sa, NULL) != 0)
    {
      puts ("sigaction failed");
      return 1;
    }

  puts ("sa_flags = SA_SIGINFO test");
  if (do_one_test20 ())
    return 1;

  sa.sa_sigaction = (void (*)(int, siginfo_t *, void *)) sh20;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

  if (sigaction (SIGHUP, &sa, NULL) != 0)
    {
      puts ("sigaction failed");
      return 1;
    }

  puts ("sa_flags = SA_SIGINFO|SA_ONSTACK test");
  if (do_one_test20 ())
    return 1;
#endif

  return 0;
}

