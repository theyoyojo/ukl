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
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define printf printk
#define puts printk

pthread_cond_t cv18 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock18 = PTHREAD_MUTEX_INITIALIZER;
bool exiting18;
int fd18, spins18, nn18;
enum { count18 = 8 };		/* Number of worker threads.  */

void *
tf18 (void *id)
{
  pthread_mutex_lock (&lock18);

  if ((long) id == 0)
    {
      while (!exiting18)
	{
	  if ((spins18++ % 1000) == 0)
	    write (fd18, ".", 1);
	  pthread_mutex_unlock (&lock18);

	  pthread_mutex_lock (&lock18);
	  int njobs = rand () % (count18 + 1);
	  nn18 = njobs;
	  if ((rand () % 30) == 0)
	    pthread_cond_broadcast (&cv18);
	  else
	    while (njobs--)
	      pthread_cond_signal (&cv18);
	}

      pthread_cond_broadcast (&cv18);
    }
  else
    {
      while (!exiting18)
	{
	  while (!nn18 && !exiting18)
	    pthread_cond_wait (&cv18, &lock18);
	  --nn18;
	  pthread_mutex_unlock (&lock18);

	  pthread_mutex_lock (&lock18);
	}
    }

  pthread_mutex_unlock (&lock18);
  return NULL;
}

int
do_test18 (void)
{
  fd18 = open ("/dev/null", O_WRONLY);
  if (fd18 < 0)
    {
      printf ("couldn't open /dev/null, %m\n");
      return 1;
    }

  pthread_t th[count18 + 1];
  pthread_attr_t attr;
  int i, ret, sz;
  pthread_attr_init (&attr);
  sz = sysconf (_SC_PAGESIZE);
  if (sz < PTHREAD_STACK_MIN)
	  sz = PTHREAD_STACK_MIN;
  pthread_attr_setstacksize (&attr, sz);

  for (i = 0; i <= count18; ++i)
    if ((ret = pthread_create (&th[i], &attr, tf18, (void *) (long) i)) != 0)
      {
	errno = ret;
	printf ("pthread_create %d failed: %m\n", i);
	return 1;
      }

  struct timespec ts = { .tv_sec = 20, .tv_nsec = 0 };
  while (nanosleep (&ts, &ts) != 0);

  pthread_mutex_lock (&lock18);
  exiting18 = true;
  pthread_mutex_unlock (&lock18);

  for (i = 0; i < count18; ++i)
    pthread_join (th[i], NULL);

  close (fd18);
  return 0;
}

#define TIMEOUT 40
