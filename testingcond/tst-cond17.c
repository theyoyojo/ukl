#define UNLOCK_AFTER_BROADCAST 1

/* Copyright (C) 2004-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2004.

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
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define printf printk
#define puts printk

pthread_cond_t cv17 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock17 = PTHREAD_MUTEX_INITIALIZER;
bool n17, exiting17;
FILE *f17;
enum { count17 = 8 };		/* Number of worker threads.  */

void *
tf17 (void *dummy)
{
  bool loop = true;

  while (loop)
    {
      pthread_mutex_lock (&lock17);
      while (n17 && !exiting17)
	pthread_cond_wait (&cv17, &lock17);
      n17 = true;
      pthread_mutex_unlock (&lock17);

      fputs (".", f17);

      pthread_mutex_lock (&lock17);
      n17 = false;
      if (exiting17)
	loop = false;
#ifdef UNLOCK_AFTER_BROADCAST
      pthread_cond_broadcast (&cv17);
      pthread_mutex_unlock (&lock17);
#else
      pthread_mutex_unlock (&lock17);
      pthread_cond_broadcast (&cv17);
#endif
    }

  return NULL;
}

int
do_test17 (void)
{
  f17 = fopen ("/dev/null", "w");
  if (f17 == NULL)
    {
      printf ("couldn't open /dev/null, %m\n");
      return 1;
    }

  pthread_t th[count17];
  pthread_attr_t attr;
  int i, ret, sz;
  pthread_attr_init (&attr);
  sz = sysconf (_SC_PAGESIZE);
  if (sz < PTHREAD_STACK_MIN)
	  sz = PTHREAD_STACK_MIN;
  pthread_attr_setstacksize (&attr, sz);
  for (i = 0; i < count17; ++i)
    if ((ret = pthread_create (&th[i], &attr, tf17, NULL)) != 0)
      {
	errno = ret;
	printf ("pthread_create %d failed: %m\n", i);
	return 1;
      }

  struct timespec ts = { .tv_sec = 20, .tv_nsec = 0 };
  while (nanosleep (&ts, &ts) != 0);

  pthread_mutex_lock (&lock17);
  exiting17 = true;
  pthread_mutex_unlock (&lock17);

  for (i = 0; i < count17; ++i)
    pthread_join (th[i], NULL);

  fclose (f17);
  return 0;
}


