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

pthread_cond_t cv16 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock16 = PTHREAD_MUTEX_INITIALIZER;
bool n16, exiting16;
FILE *f16;
enum { count16 = 8 };		/* Number of worker threads.  */

void *
tf16 (void *dummy)
{
  bool loop = true;

  while (loop)
    {
      pthread_mutex_lock (&lock16);
      while (n16 && !exiting16)
	pthread_cond_wait (&cv16, &lock16);
      n16 = true;
      pthread_mutex_unlock (&lock16);

      fputs (".", f16);

      pthread_mutex_lock (&lock16);
      n16 = false;
      if (exiting16)
	loop = false;
#ifdef UNLOCK_AFTER_BROADCAST
      pthread_cond_broadcast (&cv16);
      pthread_mutex_unlock (&lock16);
#else
      pthread_mutex_unlock (&lock16);
      pthread_cond_broadcast (&cv16);
#endif
    }

  return NULL;
}

int
do_test16 (void)
{
  f16 = fopen ("/dev/null", "w");
  if (f16 == NULL)
    {
      printf ("couldn't open /dev/null, %m\n16");
      return 1;
    }

  pthread_t th[count16];
  pthread_attr_t attr;
  int i, ret, sz;
  pthread_attr_init (&attr);
  sz = sysconf (_SC_PAGESIZE);
  if (sz < PTHREAD_STACK_MIN)
	  sz = PTHREAD_STACK_MIN;
  pthread_attr_setstacksize (&attr, sz);
  for (i = 0; i < count16; ++i)
    if ((ret = pthread_create (&th[i], &attr, tf16, NULL)) != 0)
      {
	errno = ret;
	printf ("pthread_create %d failed: %m\n16", i);
	return 1;
      }

  struct timespec ts = { .tv_sec = 20, .tv_nsec = 0 };
  while (nanosleep (&ts, &ts) != 0);

  pthread_mutex_lock (&lock16);
  exiting16 = true;
  pthread_mutex_unlock (&lock16);

  for (i = 0; i < count16; ++i)
    pthread_join (th[i], NULL);

  fclose (f16);
  return 0;
}

#define TIMEOUT 40
