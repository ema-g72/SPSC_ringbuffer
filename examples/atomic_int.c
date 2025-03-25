#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_CYCLES 2000000

#define CHECK(x) if((x) != 0) { printf("Error at line %d\n", __LINE__); exit(EXIT_FAILURE); }

int counter;
atomic_int counter_atomic;

static void *increment_function(void *arg)
{
   (void) arg;

   for(int i=0; i<NUM_CYCLES; ++i)
   {
      ++counter;
      ++counter_atomic;
   }

   return NULL;   
}

int main()
{
   pthread_attr_t attr;
   pthread_t tid1;
   pthread_t tid2;

   counter = 0;
   counter_atomic = 0;

   /* Create a producer and a consumer thread. */
   CHECK( pthread_attr_init(&attr) );

   CHECK( pthread_create(&tid1, &attr, increment_function, NULL) );
   CHECK( pthread_create(&tid2, &attr, increment_function, NULL) );

   /* Wait for completion. */
   pthread_join(tid1, NULL);
   pthread_join(tid2, NULL);
 
   printf("expected = %d, counter = %d, counter_atomic = %d\n", 2*NUM_CYCLES, counter, counter_atomic);

   return EXIT_SUCCESS;
}