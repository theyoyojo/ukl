/* Benchmark malloc and free functions.
   Copyright (C) 2013-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>

extern int printk(const char *fmt, ...);
#define printf printk

static void *
benchmark_thread (int thn)
{
	int i = 0;
	for(i = 0; i < 1000; i++){
        char *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (addr > 0){
			char c = thn + '0';
			*addr = c;
	        	char a = *((char *)addr);
        		printf("Thread = %d run = %d addr = 0x%lx and data =  %c\n",thn, i, addr, a);
		}
	}
        return;
}

static void
do_benchmark ()
{
      int num_threads = 5;

      pthread_t threads[num_threads];

      for (size_t i = 0; i < num_threads; i++)
	{
	  pthread_create(&threads[i], NULL, benchmark_thread, i);
	}

      for (size_t i = 0; i < num_threads; i++)
	{
	  pthread_join(threads[i], NULL);
	}
}

int
main ()
{
  do_benchmark ();

  return 0;
}
