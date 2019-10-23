/* Copyright (C) 2003-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2003.

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define printf printk
#define puts printk


static pthread_barrier_t b16a;
static int fd16;
static int called16;


static void
cl16 (void *arg)
{
  called16 = 1;
}


static void *
tf16 (void *arg)
{
  int r = pthread_barrier_wait (&b16a);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("child thread: barrier_wait failed");
      exit (1);
    }

  pthread_cleanup_push (cl16, NULL);

  /* This call should never return.  */
  (void) lockf (fd16, F_LOCK, 0);

  pthread_cleanup_pop (0);

  return NULL;
}


int
do_test16 (void)
{
  char fname[] = "/tmp/cancel16XXXXXX";
  fd16 = mkstemp (fname);
  if (fd16 == -1)
    {
      puts ("mkstemp failed");
      return 1;
    }
  unlink (fname);

  char mem[sizeof (pthread_barrier_t)];
  memset (mem, '\0', sizeof (mem));
  if (TEMP_FAILURE_RETRY (pwrite (fd16, mem, sizeof (mem), 0)) != sizeof (mem))
    {
      puts ("pwrite failed");
      return 1;
    }

  void *p = mmap (NULL, sizeof (mem), PROT_READ|PROT_WRITE, MAP_SHARED, fd16, 0);
  if (p == MAP_FAILED)
    {
      puts ("mmap failed");
      return 1;
    }
  pthread_barrier_t *b = (pthread_barrier_t *) p;

  pthread_barrierattr_t ba;
  if (pthread_barrierattr_init (&ba) != 0)
    {
      puts ("barrierattr_init failed");
      return 1;
    }
  if (pthread_barrierattr_setpshared (&ba, 1) != 0)
    {
      puts ("barrierattr_setshared failed");
      return 1;
    }

  if (pthread_barrier_init (b, &ba, 2) != 0)
    {
      puts ("1st barrier_init failed");
      return 1;
    }
  if (pthread_barrierattr_destroy (&ba) != 0)
    {
      puts ("barrier_destroy failed");
      return 1;
    }

  pid_t pid = fork ();
  if (pid == 0)
    {
      /* Child.  Lock the file and wait.  */
      if (lockf (fd16, F_LOCK, 0) != 0)
	{
	  puts ("child process: lockf failed");
	  _exit (1);
	}

      int r = pthread_barrier_wait (b);
      if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("child process: 1st barrier_wait failed");
	  _exit (1);
	}

      /* Make sure the process dies.  */
      alarm (5);

      r = pthread_barrier_wait (b);
      if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("child process: 2nd barrier_wait failed");
	  _exit (1);
	}

      _exit (0);
    }
  if (pid == -1)
    {
      puts ("fork failed");
      return 1;
    }

  int r = pthread_barrier_wait (b);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("main: 1st barrier_wait failed");
      _exit (1);
    }

  if (pthread_barrier_init (&b16a, NULL, 2) != 0)
    {
      puts ("2nd barrier_init failed");
      return 1;
    }

  pthread_t th;
  if (pthread_create (&th, NULL, tf16, NULL) != 0)
    {
      puts ("create failed");
      return 1;
    }

  r = pthread_barrier_wait (&b16a);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("main: 2nd barrier_wait failed");
      return 1;
    }

  /* Delay.  */
  sleep (1);

  if (pthread_cancel (th) != 0)
    {
      puts ("cancel failed");
      return 1;
    }

  void *result;
  if (pthread_join (th, &result) != 0)
    {
      puts ("join failed");
      return 1;
    }
  if (result != PTHREAD_CANCELED)
    {
      puts ("thread not canceled");
      return 1;
    }
  if (called16 == 0)
    {
      puts ("cleanup handler not called16");
      return 1;
    }

  r = pthread_barrier_wait (b);
  if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("main: 3rd barrier_wait failed");
      return 1;
    }

  int status;
  if (TEMP_FAILURE_RETRY (waitpid (pid, &status, 0)) != pid)
    {
      puts ("waitpid failed");
      return 1;
    }
  if (WEXITSTATUS (status) != 0)
    {
      printf ("child process exits with %d\n", WEXITSTATUS (status));
      return 1;
    }

  if (lockf (fd16, F_LOCK, 0) != 0)
    {
      puts ("main: lockf failed");
      return 1;
    }

  return 0;
}

