/*
   Thread safe, lock free SPSC (Single Producer, Single Consumer) ring buffer.    
   
   The producer shall obtain a free buffer pointer with the function "spsc_ringbuff_get_write_buffer"
   and push into the circular FIFO with "spsc_ringbuff_add_element". The pointer shall not be used after 
   the insertion.

   The consumer gets a buffer with the function "spsc_ringbuff_get_read_buffer", the pointer can be used
   until the element is released with the call to "spsc_ringbuff_remove_element".
 
   The API allocates dynanic memory for the internal structure and user buffers. These are deallocated by 
   the function "spsc_ringbuff_destroy".
   
*/
#ifndef _SPSC_RINGBUFF_H_
#define _SPSC_RINGBUFF_H_

/* Return type. */
typedef enum spsc_ringbuff_sts_t
{
   SPSC_RINGBUFF_OK,
   SPSC_RINGBUFF_MEM_ERROR,
   SPSC_RINGBUFF_INVALID

} SPSC_RINGBUFF_STS;

/* Ring buffer identifier. */
typedef struct spsc_ringbuff_impl_t * SPSC_RINGBUFF_ID;

/* Create a new ring buffer. This data structure can be used with only one producer and one consumer.
   This function allocates an array of "capacity" elements, each of size "elemsize". No further dynamic 
   allocation is done.
   The id shall be deallocated with "spsc_ringbuff_destroy".
    return: 
        SPSC_RINGBUFF_OK if success.
    parameters:
        id: ring buffer identifier (output).
        capacity: number of elements allocated in the ring buffer.
        elemsize: size, in bytes, of each element.
*/
SPSC_RINGBUFF_STS spsc_ringbuff_create(SPSC_RINGBUFF_ID *id, int capacity, int elemsize);

/* Reset the ring buffer. Resets the internal indexes, after this call the ringbuffer is empty.
   return: 
      SPSC_RINGBUFF_OK if success.
   parameters:
      id: ring buffer identifier.
*/
SPSC_RINGBUFF_STS spsc_ringbuff_reset(SPSC_RINGBUFF_ID id);

/* Check if ring buffer is full.
   return: 
      SPSC_RINGBUFF_OK if success.
   parameters:
      id: ring buffer identifier.
      isfull: 1 if full, 0 otherwise (output).
*/
SPSC_RINGBUFF_STS spsc_ringbuff_isfull(SPSC_RINGBUFF_ID id, int *isfull);

/* Check if ring buffer is empty.
   return: 
      SPSC_RINGBUFF_OK if success.
   parameters:
      id: ring buffer identifier.
      isempty: 1 if empty, 0 otherwise (output).
*/
SPSC_RINGBUFF_STS spsc_ringbuff_isempty(SPSC_RINGBUFF_ID id, int *isempty);

/* Return the current number of elements in the ring buffer.
   return: 
      SPSC_RINGBUFF_OK if success.
   parameters:
      id: ring buffer identifier.
      num: number of elements (output).
*/
SPSC_RINGBUFF_STS spsc_ringbuff_num_elements(SPSC_RINGBUFF_ID id, int *num);

/* Return the ring buffer capacity (the max number of elements).
   return: 
      SPSC_RINGBUFF_OK if success.      
   parameters:
      id: ring buffer identifier.
      capacity: max number of elements (output).
*/
SPSC_RINGBUFF_STS spsc_ringbuff_capacity(SPSC_RINGBUFF_ID id, int *capacity);

/* Return the size, in bytes, of each element.
   return: 
      SPSC_RINGBUFF_OK if success.      
   parameters:
      id: ring buffer identifier.
      elemsize: size, in bytes of an ring buffer element (output).
*/
SPSC_RINGBUFF_STS spsc_ringbuff_element_size(SPSC_RINGBUFF_ID id, int *elemsize);

/* Return an available buffer for writing. This is a safe buffer for the producer, it will be  
   "inserted" into the ringbuffer with the function ringbuff_add_element.
   If the ringbuffer is of blocking type this function blocks when the ring is full.
   return: 
      pointer to the write buffer, NULL in case of error or if buffer is full (in case of non blocking 
      and not overwrite).
   parameters:
      id: ring buffer identifier.
      overwrite: 0=do not overwrite if the rinbuffer is full, 1=overwrite (Only for non-blocking type).
      overwritten: 1=if the ringbuffer was full and the buffer will be overwritten (output, can be NULL
                   in case of blocking type).
*/
char *spsc_ringbuff_get_write_buffer(SPSC_RINGBUFF_ID id, int overwrite, int *overwritten);

/* Insert the write buffer into the ring.
   return: 
      element size, <0 otherwise.
   parameters:
      id: ring buffer identifier.
*/
SPSC_RINGBUFF_STS spsc_ringbuff_add_element(SPSC_RINGBUFF_ID id);

/* Return an available buffer for reading (in FIFO order). This is a safe buffer for the consumer,
   it will be set as "free" into the ringbuffer with the function ringbuff_remove_element.
   If the ringbuffer is blocking
   return: 
      pointer to the read buffer, NULL in case of error or if the buffer is full (non-blocking).
   parameters:
      id: ring buffer identifier.
*/
char *spsc_ringbuff_get_read_buffer(SPSC_RINGBUFF_ID id);

/* Free the read buffer from the ring.
   return: 
      0 if success, <0 otherwise.
   parameters:
      id: ring buffer identifier.
*/
SPSC_RINGBUFF_STS spsc_ringbuff_remove_element(SPSC_RINGBUFF_ID id);

/* Destroy the ringbuffer and release the internal resources.
   return: 
      0 if success, <0 otherwise.
   parameters:
      id: ring buffer identifier.
*/
SPSC_RINGBUFF_STS spsc_ringbuff_destroy(SPSC_RINGBUFF_ID id);

#endif