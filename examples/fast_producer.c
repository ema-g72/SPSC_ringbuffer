#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "spsc_ringbuff.h"

#define NUM_ELEMENTS 10
#define NUM_CYCLES 30

#define CHECK(x) if((x) != 0) { printf("Error at line %d\n", __LINE__); exit(EXIT_FAILURE); }

static void *producer(void *arg)
{
   int *ptr;
   SPSC_RINGBUFF_ID rb = (SPSC_RINGBUFF_ID) arg;
   int i = 0;

   while(i<NUM_CYCLES)
   {
      /* Get a buffer, if available.*/
      if((ptr = (int *)spsc_ringbuff_get_write_buffer(rb, 0, NULL )) != NULL)
      {
         /* Use the buffer. */
         *ptr = i;
         /* Release the buffer (insert into the fifo). */
         CHECK( spsc_ringbuff_add_element(rb) );
         printf("producer, write %d at %p\n", i, ptr);
         
         i++;
      }
      usleep(10000);
   }
   return NULL;   
}

static void *consumer(void *arg)
{
   int *ptr;
   SPSC_RINGBUFF_ID rb = (SPSC_RINGBUFF_ID) arg;
   int data;
   int i=0;

   while(i<NUM_CYCLES)
   {
      /* Get a read buffer, if available.*/
      if((ptr = (int *)spsc_ringbuff_get_read_buffer(rb)) != NULL)
      {
         /* Use the buffer. */
         data = *ptr;
         /* Release the buffer (remove from the fifo). */
         CHECK( spsc_ringbuff_remove_element(rb) );
         printf("consumer, read %d at %p\n", data, ptr);

         i++;
      }
      usleep(200000);
   }
   return NULL;   
}

int main()
{
   pthread_attr_t attr;
   pthread_t prod_tid;
   pthread_t cons_tid;
   SPSC_RINGBUFF_ID rb;

   CHECK( spsc_ringbuff_create(&rb, NUM_ELEMENTS, sizeof(int)) );
 
   CHECK( pthread_attr_init(&attr) );

   CHECK( pthread_create(&prod_tid, &attr, producer, (void *) (rb)) );
   CHECK( pthread_create(&cons_tid, &attr, consumer, (void *) (rb)) );

   pthread_join(prod_tid, NULL);
   pthread_join(cons_tid, NULL);

   CHECK( spsc_ringbuff_destroy(rb) );
 
   return EXIT_SUCCESS;
}