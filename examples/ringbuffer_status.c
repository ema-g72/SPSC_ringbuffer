#include <stdio.h>
#include <stdlib.h>
#include "spsc_ringbuff.h"

#define RB1_NUM_ELEMENTS 10
#define RB1_ELEM_SIZE 32

#define RB2_NUM_ELEMENTS 200
#define RB2_ELEM_SIZE 1024

#define CHECK(x) if((x) != SPSC_RINGBUFF_OK) { printf("Error at line %d\n", __LINE__); exit(EXIT_FAILURE); }

void get_status(SPSC_RINGBUFF_ID rb, int *nelements, int *full, int *empty)
{
   CHECK( spsc_ringbuff_num_elements(rb, nelements));
   CHECK( spsc_ringbuff_isfull(rb, full));
   CHECK( spsc_ringbuff_isempty(rb, empty));
}

void produce_elements(SPSC_RINGBUFF_ID rb, int num_elements, int overwrite)
{
   char *ptr;
   int nelements, full, empty;
   int overwritten;

   for(int i=0; i<num_elements; ++i)
   {
      if((ptr = spsc_ringbuff_get_write_buffer(rb, overwrite, &overwritten )) != NULL)
      {
         CHECK( spsc_ringbuff_add_element(rb) );
      }
      get_status(rb, &nelements, &full, &empty);
      
      printf("   num elements: %d, is full: %d, is empty: %d, overwritten: %d, (ptr: %p)\n", nelements, full, empty, overwritten, ptr);
   }
}

void consume_elements(SPSC_RINGBUFF_ID rb, int num_elements)
{
   char *ptr;
   int nelements, full, empty;

   for(int i=0; i<num_elements; ++i)
   {
      if( (ptr = spsc_ringbuff_get_read_buffer(rb)) != NULL)
      {
         CHECK( spsc_ringbuff_remove_element(rb) );
      }
      get_status(rb, &nelements, &full, &empty);
      
      printf("   num elements: %d, is full: %d, is empty: %d, (ptr: %p)\n", nelements, full, empty, ptr);
   }
}

int main()
{
   SPSC_RINGBUFF_ID rb1;
   SPSC_RINGBUFF_ID rb2;
   int capacity1, capacity2, elemsize1, elemsize2;
   int nelements, full, empty;

   CHECK( spsc_ringbuff_create(&rb1, RB1_NUM_ELEMENTS, RB1_ELEM_SIZE) );
   CHECK( spsc_ringbuff_create(&rb2, RB2_NUM_ELEMENTS, RB2_ELEM_SIZE) );

   CHECK( spsc_ringbuff_capacity(rb1, &capacity1) );
   CHECK( spsc_ringbuff_element_size(rb1, &elemsize1) );
   printf("Ringbuffer #1, capacity: %d, size: %d\n", capacity1, elemsize1);

   CHECK( spsc_ringbuff_capacity(rb2, &capacity2) );
   CHECK( spsc_ringbuff_element_size(rb2, &elemsize2) );
   printf("Ringbuffer #2, capacity: %d, size: %d\n", capacity2, elemsize2);

   printf("Ringbuffer #1, produce elements do not overwrite\n");
   produce_elements(rb1, RB1_NUM_ELEMENTS+2, 0);

   printf("Ringbuffer #1, consume elements\n");
   consume_elements(rb1, RB1_NUM_ELEMENTS+2);

   printf("Ringbuffer #1, produce elements overwrite\n");
   produce_elements(rb1, RB1_NUM_ELEMENTS+2, 1);

   printf("Ringbuffer #1, reset\n");
   CHECK( spsc_ringbuff_reset(rb1));
   get_status(rb1, &nelements, &full, &empty);
 
   printf("Ringbuffer #1, num elements: %d, is full: %d, is empty: %d\n", nelements, full, empty);

   CHECK( spsc_ringbuff_destroy(rb1) );
   CHECK( spsc_ringbuff_destroy(rb2) );

   return EXIT_SUCCESS;
}