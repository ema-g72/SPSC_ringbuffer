#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "spsc_ringbuff.h"

#define NUM_ELEMENTS 50
#define ARRAY_SIZE 2000000

#define NUM_CYCLES 2000

#define CHECK(x) if((x) != 0) { printf("Error at line %d\n", __LINE__); exit(EXIT_FAILURE); }

static void fill_buffer(int *buff, int value)
{
   for(int i=0; i<ARRAY_SIZE; ++i)
   {
      buff[i] = value;
   }
}

static void check_buffer(int *buff, int value)
{
   for(int i=0; i<ARRAY_SIZE; ++i)
   {
      if( buff[i] != value )
      {
         printf("Data mismatch %d/%d\n", buff[i], value);
         exit(EXIT_FAILURE);
      }
   }
}

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
         /* Use the buffer: fill the array with current index. */
         fill_buffer(ptr, i);         
         /* Release the buffer (insert into the FIFO). */
         CHECK( spsc_ringbuff_add_element(rb) );
         
         if(i%100 == 0)
         {  
            printf("producer: setting array elements to %d\n", i);            
         }

         i++;
      }
   }
   return NULL;   
}

static void *consumer(void *arg)
{
   int *ptr;
   SPSC_RINGBUFF_ID rb = (SPSC_RINGBUFF_ID) arg;
   int i=0;
   int isfull = 0;

   /* Wait until the buffer is full. */
   while(!isfull)
   {
      usleep(0);
      spsc_ringbuff_isfull(rb, &isfull);      
   }
   
   while(i<NUM_CYCLES)
   {
      /* Get a read buffer, if available.*/
      if((ptr = (int *)spsc_ringbuff_get_read_buffer(rb)) != NULL)
      {
         /* Use the buffer: checks that all the integers have the same value.  */
         check_buffer(ptr, i);
         /* Release the buffer (remove from FIFO). */
         CHECK( spsc_ringbuff_remove_element(rb) );

         i++;
      }
   }
   return NULL;   
}

int main()
{
   pthread_attr_t attr;
   pthread_t prod_tid;
   pthread_t cons_tid;
   SPSC_RINGBUFF_ID rb;

   /* Create a ring buffer shared between two threads.*/
   CHECK( spsc_ringbuff_create(&rb, NUM_ELEMENTS, ARRAY_SIZE*sizeof(int)) );
 
   /* Create a producer and a consumer thread. */
   CHECK( pthread_attr_init(&attr) );

   CHECK( pthread_create(&prod_tid, &attr, producer, (void *) (rb)) );
   CHECK( pthread_create(&cons_tid, &attr, consumer, (void *) (rb)) );

   /* Wait for completion. */
   pthread_join(prod_tid, NULL);
   pthread_join(cons_tid, NULL);

   CHECK( spsc_ringbuff_destroy(rb) );
 
   return EXIT_SUCCESS;
}