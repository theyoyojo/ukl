/* Test sem_wait cancellation for uncontended case.
   Copyright (C) 2003-2019 Free Software Foundation, Inc.
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
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define printf printk
#define puts printk


static pthread_barrier_t bar12;
static sem_t sem12;


static void
cleanup12 (void *arg)
{
  static int ncall;

  if (++ncall != 1)
    {
      puts ("second call to cleanup12");
      exit (1);
    }
}


static void *
tf12 (void *arg)
{
  pthread_cleanup_push (cleanup12, NULL);

  int e = pthread_barrier_wait (&bar12);
  if (e != 0 && e != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("error: tf12: 1st barrier_wait failed");
      exit (1);
    }

  /* This call should be cancelable.  */
  sem_wait (&sem12);

  pthread_cleanup_pop (0);

  return NULL;
}


int
do_test12 (void)
{
  pthread_t th;

  if (pthread_barrier_init (&bar12, NULL, 2) != 0)
    {
      puts ("error: barrier_init failed");
      exit (1);
    }

  /* A value higher than 0 will check for uncontended pthread cancellation,
     where the sem_wait operation will return immediatelly.  */
  if (sem_init (&sem12, 0, 1) != 0)
    {
      puts ("error: sem_init failed");
      exit (1);
    }

  if (pthread_create (&th, NULL, tf12, NULL) != 0)
    {
      puts ("error: create failed");
      exit (1);
    }

  if (pthread_cancel (th) != 0)
    {
      puts ("error: pthread_cancel failed");
      exit (1);
    }

  int e = pthread_barrier_wait (&bar12);
  if (e != 0 && e != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("error: 1st barrier_wait failed");
      exit (1);
    }

  void *r;
  if (pthread_join (th, &r) != 0)
    {
      puts ("error: pthread_join failed");
      exit (1);
    }

  if (r != PTHREAD_CANCELED)
    {
      puts ("error: thread not canceled");
      exit (1);
    }

  return 0;
}


