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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define printf printk
#define puts printk


static pthread_barrier_t bar8;

static int global8;


static void
cleanup8 (void *arg)
{
  global8 = 1;
}


static void *
tf8 (void *arg)
{
  /* Enable cancellation, but defer it.  */
  if (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL) != 0)
    {
      puts ("setcancelstate failed");
      exit (1);
    }
  if (pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL) != 0)
    {
      puts ("setcanceltype failed");
      exit (1);
    }

  /* Add cleanup8 handler.  */
  pthread_cleanup_push (cleanup8, NULL);

  /* Synchronize with the main thread.  */
  int r = pthread_barrier_wait (&bar8);
  if (r != 0 && r!= PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("tf8: first barrier_wait failed");
      exit (1);
    }

  /* And again.  Once this is done the main thread should have canceled
     this thread.  */
  r = pthread_barrier_wait (&bar8);
  if (r != 0 && r!= PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("tf8: second barrier_wait failed");
      exit (1);
    }

  /* Remove the cleanup8 handler without executing it.  */
  pthread_cleanup_pop (0);

  /* Now react on the cancellation.  */
  pthread_testcancel ();

  /* This call should never return.  */
  return NULL;
}


int
do_test8 (void)
{
  if (pthread_barrier_init (&bar8, NULL, 2) != 0)
    {
      puts ("barrier_init failed");
      exit (1);
    }

  pthread_t th;
  if (pthread_create (&th, NULL, tf8, NULL) != 0)
    {
      puts ("pthread_create failed");
      return 1;
    }

  int r = pthread_barrier_wait (&bar8);
  if (r != 0 && r!= PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("first barrier_wait failed");
      exit (1);
    }

  if (pthread_cancel (th) != 0)
    {
      puts ("pthread_cancel failed");
      return 1;
    }

  r = pthread_barrier_wait (&bar8);
  if (r != 0 && r!= PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("second barrier_wait failed");
      exit (1);
    }

  void *result;
  if (pthread_join (th, &result) != 0)
    {
      puts ("pthread_join failed");
      return 1;
    }

  if (result != PTHREAD_CANCELED)
    {
      puts ("thread was not canceled");
      exit (1);
    }

  if (global8 != 0)
    {
      puts ("cancellation handler has been called");
      exit (1);
    }

  return 0;
}

