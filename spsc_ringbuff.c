/*
   Thread safe, lock free SPSC (Single Producer, Single Consumer) ring buffer. 
   It must be used with only one producer and only one consumer thread.    
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include "spsc_ringbuff.h"

/* Opaque type representing a ring buffer. */
struct spsc_ringbuff_impl_t {
   char *buffer;
   int capacity;
   int elemsize;
   char full;
   /* these shall be atomic */
   atomic_int head;
   atomic_int tail;
};

SPSC_RINGBUFF_STS spsc_ringbuff_create(SPSC_RINGBUFF_ID *id, int capacity, int elemsize)
{
   /* Allocate the ring buffer object, set all elements to zero. */
   if( (*id = (struct spsc_ringbuff_impl_t *)calloc(1, sizeof(struct spsc_ringbuff_impl_t))) == NULL)
   {
      return SPSC_RINGBUFF_MEM_ERROR;
   }

   (*id)->capacity = capacity;
   (*id)->elemsize = elemsize;

   /* Allocate data buffer. */
   if( ((*id)->buffer = calloc(capacity, elemsize)) == NULL)
   {
      return SPSC_RINGBUFF_MEM_ERROR;
   }

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_reset(SPSC_RINGBUFF_ID id)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   id->full = (char)0;
   id->head = 0;
   id->tail = 0;

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_isfull(SPSC_RINGBUFF_ID id, int *isfull)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   *isfull = id->full;

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_isempty(SPSC_RINGBUFF_ID id, int *isempty)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }
   
   *isempty = (!id->full && (id->head == id->tail));

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_capacity(SPSC_RINGBUFF_ID id, int *capacity)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   *capacity = id->capacity;

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_element_size(SPSC_RINGBUFF_ID id, int *elemsize)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   *elemsize = id->elemsize;

   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_num_elements(SPSC_RINGBUFF_ID id, int *num)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   if(id->full)
   {
      *num = id->capacity;
   }
   else
   {
      int head = id->head;
      int tail = id->tail;

      if(head >= tail)
      {
         *num = (head - tail);
      }
      else
      {
         *num = (id->capacity - tail + head);
      }
   }

   return SPSC_RINGBUFF_OK;
}

char *spsc_ringbuff_get_write_buffer(SPSC_RINGBUFF_ID id, int overwrite, int *overwritten)
{
   int isfull;

   if(id == NULL)
   {
      return NULL;
   }
   
   isfull = id->full;

   if(!overwrite && isfull)
   {
      if( overwritten != NULL )
      {
         *overwritten = 0;
      }

      return NULL;
   }
   else if( overwritten != NULL )
   {
      *overwritten = isfull;
   }

   /* Return the buffer at head position. */
   return id->buffer + (id->head*id->elemsize);
}

SPSC_RINGBUFF_STS spsc_ringbuff_add_element(SPSC_RINGBUFF_ID id)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }
   
   /* Adding an element means to increase the head index but if the buffer 
      is full also the tail index shall be increased. */
   if(id->full)
   {
      if(++(id->tail) == id->capacity)
      {
         id->tail = 0;
      }
   }

   /* Increase the head index. */
   if(++(id->head) == id->capacity)
   {
      id->head = 0;
   }

   /* Recompute full flag. */
   id->full = (id->head == id->tail);

   return SPSC_RINGBUFF_OK;
}

char *spsc_ringbuff_get_read_buffer(SPSC_RINGBUFF_ID id)
{
   int empty;

   if(id == NULL)
   {
      return NULL;
   }
   
   if( spsc_ringbuff_isempty(id, &empty) != SPSC_RINGBUFF_OK)
   {
      return NULL;
   }

   if(empty)
   {
      return NULL;
   }

   /* Return the buffer at tail position. */
   return id->buffer + (id->tail*id->elemsize);
}

SPSC_RINGBUFF_STS spsc_ringbuff_remove_element(SPSC_RINGBUFF_ID id)
{
   int empty;
   SPSC_RINGBUFF_STS ret;

   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }

   if( (ret = spsc_ringbuff_isempty(id, &empty)) != SPSC_RINGBUFF_OK)
   {
      return ret;
   }
   
   if(!empty)
   {
      /* Reset full flag. */
      id->full = 0;

      /* Increase the tail index. */
      if(++(id->tail) == id->capacity)
      {
         id->tail = 0;
      }
   }
   return SPSC_RINGBUFF_OK;
}

SPSC_RINGBUFF_STS spsc_ringbuff_destroy(SPSC_RINGBUFF_ID id)
{
   if(id == NULL)
   {
      return SPSC_RINGBUFF_INVALID;
   }
   
   /* Free memory. */
   if(id->buffer != NULL)
   {
      free(id->buffer);
   }

   free(id);

   return SPSC_RINGBUFF_OK;
}
