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

extern int printk(const char *fmt, ...);
#define printf printk

/* Benchmark duration in seconds.  */
#define BENCHMARK_DURATION	10
#define RAND_SEED		88

#ifndef NUM_THREADS
# define NUM_THREADS 1
#endif

/* Maximum memory that can be allocated at any one time is:

   NUM_THREADS * WORKING_SET_SIZE * MAX_ALLOCATION_SIZE

   However due to the distribution of the random block sizes
   the typical amount allocated will be much smaller.  */
#define WORKING_SET_SIZE	1024

#define MIN_ALLOCATION_SIZE	4
#define MAX_ALLOCATION_SIZE	32768

/* Get a random block size with an inverse square distribution.  */
static unsigned int
get_block_size (unsigned int rand_data)
{
  /* Inverse square.  */
  const float exponent = -2;
  /* Minimum value of distribution.  */
  const float dist_min = MIN_ALLOCATION_SIZE;
  /* Maximum value of distribution.  */
  const float dist_max = MAX_ALLOCATION_SIZE;

  float min_pow = powf (dist_min, exponent + 1);
  float max_pow = powf (dist_max, exponent + 1);

  float r = (float) rand_data / RAND_MAX;

  return (unsigned int) powf ((max_pow - min_pow) * r + min_pow,
			      1 / (exponent + 1));
}

#define NUM_BLOCK_SIZES	8000
#define NUM_OFFSETS	((WORKING_SET_SIZE) * 4)

static unsigned int random_block_sizes[NUM_BLOCK_SIZES];
static unsigned int random_offsets[NUM_OFFSETS];

static void
init_random_values (void)
{
  for (size_t i = 0; i < NUM_BLOCK_SIZES; i++)
    random_block_sizes[i] = get_block_size (rand ());

  for (size_t i = 0; i < NUM_OFFSETS; i++)
    random_offsets[i] = rand () % WORKING_SET_SIZE;
}

static unsigned int
get_random_block_size (unsigned int *state)
{
  unsigned int idx = *state;

  if (idx >= NUM_BLOCK_SIZES - 1)
    idx = 0;
  else
    idx++;

  *state = idx;

  return random_block_sizes[idx];
}

static unsigned int
get_random_offset (unsigned int *state)
{
  unsigned int idx = *state;

  if (idx >= NUM_OFFSETS - 1)
    idx = 0;
  else
    idx++;

  *state = idx;

  return random_offsets[idx];
}

void add_diff_to_sum(struct timespec *result,struct timespec a, struct timespec b)
{
        if (result->tv_nsec +a.tv_nsec < b.tv_nsec)
        {
                result->tv_nsec = 1000000000+result->tv_nsec+a.tv_nsec-b.tv_nsec;
                result->tv_sec = result->tv_sec+a.tv_sec-b.tv_sec-1;
        }
        else if (result->tv_nsec+a.tv_nsec-b.tv_nsec >=1000000000)
        {
                result->tv_nsec = result->tv_nsec+a.tv_nsec-b.tv_nsec-1000000000;
                result->tv_sec = result->tv_sec+a.tv_sec-b.tv_sec+1;
        }
        else
        {
                result->tv_nsec = result->tv_nsec + a.tv_nsec - b.tv_nsec;
                result->tv_sec = result->tv_sec +a.tv_sec - b.tv_sec;
        }
}

static volatile bool timeout;

static void
alarm_handler (int signum)
{
  timeout = true;
}

/* Allocate and free blocks in a random order.  */
static size_t
malloc_benchmark_loop (void **ptr_arr)
{
  unsigned int offset_state = 0, block_state = 0;
  size_t iters = 0;

  while (!timeout)
    {
      unsigned int next_idx = get_random_offset (&offset_state);
      unsigned int next_block = get_random_block_size (&block_state);

      free (ptr_arr[next_idx]);

      ptr_arr[next_idx] = malloc (next_block);

      iters++;
    }

  return iters;
}

struct thread_args
{
  size_t iters;
  void **working_set;
};

static void *
benchmark_thread (void *arg)
{
  struct thread_args *args = (struct thread_args *) arg;
  size_t iters;
  void *thread_set = args->working_set;
  struct timespec startTime, endTime;
  struct timespec *diffTime;
  
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  iters = malloc_benchmark_loop (thread_set);
  clock_gettime(CLOCK_MONOTONIC,&endTime);

  add_diff_to_sum(diffTime, endTime, startTime);
  printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec);
  args->iters = iters;

  return NULL;
}

static void
do_benchmark (size_t num_threads, size_t *iters)
{
  if (num_threads == 1)
    {
      struct timespec startTime, endTime;
      struct timespec* diffTime;
      void *working_set[WORKING_SET_SIZE];

      memset (working_set, 0, sizeof (working_set));
      
      clock_gettime(CLOCK_MONOTONIC, &startTime);
      *iters = malloc_benchmark_loop (working_set);
      clock_gettime(CLOCK_MONOTONIC,&endTime);
      
      add_diff_to_sum(diffTime, endTime, startTime);
      printf("Test took: %ld.%09ld seconds\n",diffTime->tv_sec, diffTime->tv_nsec);
    }
  else
    {
      struct thread_args args[num_threads];
      void *working_set[num_threads][WORKING_SET_SIZE];
      pthread_t threads[num_threads];

      memset (working_set, 0, sizeof (working_set));

      *iters = 0;

      for (size_t i = 0; i < num_threads; i++)
	{
	  args[i].working_set = working_set[i];
	  pthread_create(&threads[i], NULL, benchmark_thread, &args[i]);
	}

      for (size_t i = 0; i < num_threads; i++)
	{
	  pthread_join(threads[i], NULL);
	  *iters += args[i].iters;
	}
    }
}

int
mallocmain (int argc, char **argv)
{
  size_t iters = 0, num_threads = 2;
  struct sigaction act;

  init_random_values ();

  memset (&act, 0, sizeof (act));
  act.sa_handler = &alarm_handler;

  sigaction (SIGALRM, &act, NULL);

  alarm (BENCHMARK_DURATION);

  do_benchmark (num_threads, &iters);

  return 0;
}
