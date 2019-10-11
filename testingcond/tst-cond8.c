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
#include <time.h>
#include <sys/time.h>

#define printf printk
#define puts printk

static pthread_cond_t cond8 = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mut8 = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

static pthread_barrier_t bar8;


static void
ch8 (void *arg)
{
  int e = pthread_mutex_lock (&mut8);
  if (e == 0)
    {
      puts ("mutex not locked at all by cond_wait");
      exit (1);
    }

  if (e != EDEADLK)
    {
      puts ("no deadlock error signaled");
      exit (1);
    }

  if (pthread_mutex_unlock (&mut8) != 0)
    {
      puts ("ch8: cannot unlock mutex");
      exit (1);
    }

  puts ("ch8 done");
}


static void *
tf8a (void *p)
{
  int err;

  if (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL) != 0
      || pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL) != 0)
    {
      puts ("cannot set cancellation options");
      exit (1);
    }

  err = pthread_mutex_lock (&mut8);
  if (err != 0)
    {
      puts ("child: cannot get mutex");
      exit (1);
    }

  err = pthread_barrier_wait (&bar8);
  if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      printf ("barrier_wait returned %d\n", err);
      exit (1);
    }

  puts ("child: got mutex; waiting");

  pthread_cleanup_push (ch8, NULL);
 
  pthread_cond_wait (&cond8, &mut8);

  pthread_cleanup_pop (0);

  puts ("child: cond_wait should not have returned");

  return NULL;
}


static void *
tf8b (void *p)
{
  int err;

  if (pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL) != 0
      || pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL) != 0)
    {
      puts ("cannot set cancellation options");
      exit (1);
    }

  err = pthread_mutex_lock (&mut8);
  if (err != 0)
    {
      puts ("child: cannot get mutex");
      exit (1);
    }

  err = pthread_barrier_wait (&bar8);
  if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      printf ("barrier_wait returned %d\n", err);
      exit (1);
    }

  puts ("child: got mutex; waiting");

  pthread_cleanup_push (ch8, NULL);

  /* Current time.  */
  struct timeval tv;
  (void) gettimeofday (&tv, NULL);
  /* +1000 seconds in correct format.  */
  struct timespec ts;
  TIMEVAL_TO_TIMESPEC (&tv, &ts);
  ts.tv_sec += 1000;

  pthread_cond_timedwait (&cond8, &mut8, &ts);

  pthread_cleanup_pop (0);

  puts ("child: cond_wait should not have returned");

  return NULL;
}

int
do_test8 (void)
{
  pthread_t th;
  int err;

  printf ("&cond8 = %p\n&mut8 = %p\n", &cond8, &mut8);

  puts ("parent: get mutex");

  err = pthread_barrier_init (&bar8, NULL, 2);
  if (err != 0)
    {
      puts ("parent: cannot init barrier");
      exit (1);
    }

  puts ("parent: create child");

  err = pthread_create (&th, NULL, tf8a, NULL);
  if (err != 0)
    {
      puts ("parent: cannot create thread");
      exit (1);
    }

  puts ("parent: wait for child to lock mutex");

  err = pthread_barrier_wait (&bar8);
  if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("parent: cannot wait for barrier");
      exit (1);
    }

  err = pthread_mutex_lock (&mut8);
  if (err != 0)
    {
      puts ("parent: mutex_lock failed");
      exit (1);
    }

  err = pthread_mutex_unlock (&mut8);
  if (err != 0)
    {
      puts ("parent: mutex_unlock failed");
      exit (1);
    }

  if (pthread_cancel (th) != 0)
    {
      puts ("cannot cancel thread");
      exit (1);
    }

  void *r;
  err = pthread_join (th, &r);
  if (err != 0)
    {
      puts ("parent: failed to join");
      exit (1);
    }

  if (r != PTHREAD_CANCELED)
    {
      puts ("child hasn't been canceled");
      exit (1);
    }



  puts ("parent: create 2nd child");

  err = pthread_create (&th, NULL, tf8b, NULL);
  if (err != 0)
    {
      puts ("parent: cannot create thread");
      exit (1);
    }

  puts ("parent: wait for child to lock mutex");

  err = pthread_barrier_wait (&bar8);
  if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
    {
      puts ("parent: cannot wait for barrier");
      exit (1);
    }

  err = pthread_mutex_lock (&mut8);
  if (err != 0)
    {
      puts ("parent: mutex_lock failed");
      exit (1);
    }

  err = pthread_mutex_unlock (&mut8);
  if (err != 0)
    {
      puts ("parent: mutex_unlock failed");
      exit (1);
    }

  if (pthread_cancel (th) != 0)
    {
      puts ("cannot cancel thread");
      exit (1);
    }

  err = pthread_join (th, &r);
  if (err != 0)
    {
      puts ("parent: failed to join");
      exit (1);
    }

  if (r != PTHREAD_CANCELED)
    {
      puts ("child hasn't been canceled");
      exit (1);
    }

  puts ("done");

  return 0;
}


