/******************************************************************************
* Copyright (C) 2012 David Rusk
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 32
#define RANDOM_NUMBER_UPPER_LIMIT 99

typedef struct
{
  int buffer_values[BUFFER_SIZE];
  int head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} buffer;

buffer *init_buffer()
{
  buffer *buff = (buffer *)malloc(sizeof(buffer));
  buff->head = 0;
  buff->tail = 0;
  buff->full = 0;
  buff->empty = 1;
  buff->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(buff->mut, NULL);
  buff->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(buff->notFull, NULL);
  buff->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(buff->notEmpty, NULL);

  return buff;
}

void buffer_add(buffer *buff, int input)
{
  buff->buffer_values[buff->tail] = input;
  buff->tail++;
  if (buff->tail == BUFFER_SIZE)
    {
      buff->tail = 0;
    }
  if(buff->tail == buff->head)
    {
      buff->full = 1;
    }
  buff->empty = 0;
  pthread_cond_signal(buff->notEmpty);
}

int buffer_consume(buffer *buff)
{
  int value_consumed = buff->buffer_values[buff->head];
  buff->head++;
  if (buff->head == BUFFER_SIZE)
    {
      buff->head = 0;
    }
  if (buff->head == buff->tail)
    {
      buff->empty = 1;
    }
  buff->full = 0;
  pthread_cond_signal(buff->notFull);

  return value_consumed;
}

void destroy_buffer(buffer *buff)
{
  pthread_mutex_destroy(buff->mut);
  free(buff->mut);
  pthread_cond_destroy(buff->notFull);
  free(buff->notFull);
  pthread_cond_destroy(buff->notEmpty);
  free(buff->notEmpty);
  free(buff);
}

void seed_random_number_generator()
{
  srand((unsigned)time(NULL));
}

int generate_random_number(int limit)
{
  int num = rand() % limit + 1;
  return num;
}

void *producer_function(void *b)
{
  buffer *buff = (buffer *)b;

  int i;
  for (i = 0; i < 100; i++)
    {
      pthread_mutex_lock(buff->mut);
      if(buff->full)
	{
	  printf("buffer full, waiting...\n");
	  pthread_cond_wait(buff->notFull, buff->mut);
	}

      int random = generate_random_number(RANDOM_NUMBER_UPPER_LIMIT);

      buffer_add(buff, random);

      printf("produced %d\n", random);

      pthread_mutex_unlock(buff->mut);

      /* Sleep either 1s or 4s (50% chance for each) */
      if (generate_random_number(99) < 50)
	{
	  usleep(1000000);
	}
      else 
	{
	  usleep(4000000);
	}
    }

  return NULL;
}

void *consumer_function(void *b)
{
  buffer *buff = (buffer *)b;

  int i;
  for (i = 0; i < 100; i++)
    {
      pthread_mutex_lock(buff->mut);
      if(buff->empty)
	{
	  printf("buffer empty, waiting...\n");
	  pthread_cond_wait(buff->notEmpty, buff->mut);
	}

      int consumed = buffer_consume(buff);

      printf("consumed %d\n", consumed);

      pthread_mutex_unlock(buff->mut);

      /* Sleep either 1s or 5s (50% chance for each) */
      if (generate_random_number(99) < 50)
	{
	  usleep(1000000);
	}
      else
	{
	  usleep(5000000);
	}
    }

  return NULL;
}

int main()
{
  /* Initialize buffer to store ints */
  buffer *buff = init_buffer();

  seed_random_number_generator();

  /* Create/start threads */
  pthread_t producer, consumer;

  pthread_create(&producer, NULL, producer_function, buff);
  pthread_create(&consumer, NULL, consumer_function, buff);

  /* Join threads */
  pthread_join(producer, NULL);
  pthread_join(consumer, NULL);

  /* Free buffer/memory */
  destroy_buffer(buff);

  return 0;
}
