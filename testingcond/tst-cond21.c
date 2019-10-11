#include <sys/time.h>
#define TIMED 1
/* Copyright (C) 2104-2119 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2104.

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
#include <string.h>
#include <unistd.h>

#define printf printk
#define puts printk

#define N21 10
#define ROUNDS21 1000
static pthread_cond_t cond21 = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond21b = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mut21 = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t b21;
static int count21;

static void *
tf21 (void *p)
{
  int i;
  for (i = 0; i < ROUNDS21; ++i)
    {
      pthread_mutex_lock (&mut21);

      if (++count21 == N21)
	pthread_cond_signal (&cond21b);

#ifdef TIMED
      struct timeval tv;
      gettimeofday (&tv, NULL);
      struct timespec ts;
      /* Wait three seconds.  */
      ts.tv_sec = tv.tv_sec + 3;
      ts.tv_nsec = tv.tv_usec * 1000;
      pthread_cond_timedwait (&cond21, &mut21, &ts);
#else
      pthread_cond_wait (&cond21, &mut21);
#endif

      pthread_mutex_unlock (&mut21);

      int err = pthread_barrier_wait (&b21);
      if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("child: barrier_wait failed");
	  exit (1);
	}

      err = pthread_barrier_wait (&b21);
      if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("child: barrier_wait failed");
	  exit (1);
	}
    }

  return NULL;
}


int
do_test21 (void)
{
  if (pthread_barrier_init (&b21, NULL, N21 + 1) != 0)
    {
      puts ("barrier_init failed");
      return 1;
    }

  pthread_mutex_lock (&mut21);

  int i, j, err;
  pthread_t th[N21];
  for (i = 0; i < N21; ++i)
    if ((err = pthread_create (&th[i], NULL, tf21, NULL)) != 0)
      {
	printf ("cannot create thread %d: %s\n", i, strerror (err));
	return 1;
      }

  for (i = 0; i < ROUNDS21; ++i)
    {
      /* Make sure we discard spurious wake-ups.  */
      do
	pthread_cond_wait (&cond21b, &mut21);
      while (count21 != N21);

      if (i & 1)
        pthread_mutex_unlock (&mut21);

      if (i & 2)
	pthread_cond_broadcast (&cond21);
      else if (i & 4)
	for (j = 0; j < N21; ++j)
	  pthread_cond_signal (&cond21);
      else
	{
	  for (j = 0; j < (i / 8) % N21; ++j)
	    pthread_cond_signal (&cond21);
	  pthread_cond_broadcast (&cond21);
	}

      if ((i & 1) == 0)
        pthread_mutex_unlock (&mut21);

      err = pthread_cond_destroy (&cond21);
      if (err)
	{
	  printf ("pthread_cond_destroy failed: %s\n", strerror (err));
	  return 1;
	}

      /* Now clobber the cond21 variable which has been successfully
         destroyed above.  */
      memset (&cond21, (char) i, sizeof (cond21));

      err = pthread_barrier_wait (&b21);
      if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("parent: barrier_wait failed");
	  return 1;
	}

      pthread_mutex_lock (&mut21);

      err = pthread_barrier_wait (&b21);
      if (err != 0 && err != PTHREAD_BARRIER_SERIAL_THREAD)
	{
	  puts ("parent: barrier_wait failed");
	  return 1;
	}

      count21 = 0;
      err = pthread_cond_init (&cond21, NULL);
      if (err)
	{
	  printf ("pthread_cond_init failed: %s\n", strerror (err));
	  return 1;
	}
    }

  for (i = 0; i < N21; ++i)
    if ((err = pthread_join (th[i], NULL)) != 0)
      {
	printf ("failed to join thread %d: %s\n", i, strerror (err));
	return 1;
      }

  puts ("done");

  return 0;
}

