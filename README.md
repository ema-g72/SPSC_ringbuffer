# Description
A ring-buffer is a data structure with fixed size used to store data in a continuous loop, where the first elements follows the last one, like in a circular array. It is often used in embedded systems for transferring data from a producer thread to a consumer thread. This implementation guarantees the consistency of data and a thread safe operation without using mutexes or other lock mechanism. This is realized with the usage of atomic access to the head and tail indexes of the internal array. The ring-buffer shall be shared between a single producer and a single consumer.

## Features
- Data read/write in FIFO manner.
- Direct access to the internal buffer, no extra copy is needed.
- Optional data overwrite in case of ring-buffer full.
  
# Usage
Use `spsc_ringbuff_create` to create a ring-buffer object, it allocates the required memory for the internal structure and for the user buffer. The memory is deallocated by `spsc_ringbuff_destroy`.

```c
#define NUM_ELEMENTS 16
#define ELEMENT_SIZE (1024*sizeof(double))

SPSC_RINGBUFF_ID rb;

spsc_ringbuff_create(&rb, NUM_ELEMENTS, ELEMENT_SIZE);

// ...

spsc_ringbuff_destroy(&rb);
```

The producer gets a buffer for writing, the second parameter of `spsc_ringbuff_get_write_buffer` indicates if the oldest buffer shall be overwritten in case of ring buffer full. If the overwrite flag is false and no elements are available the function returns NULL.
When the writing is completed, the buffer shall be added to the FIFO with `spsc_ringbuff_add_element`, this call makes the element available for the consumer.

```c
if((ptr = (int *)spsc_ringbuff_get_write_buffer(rb, 0, NULL )) != NULL)
{
   // Use the buffer for writing.
   // *ptr = ...

   // Release the buffer (insert into the FIFO).
   spsc_ringbuff_add_element(rb);
}
```   

The consumer gets an available buffer for reading with `spsc_ringbuff_get_read_buffer`. The function returns NULL in case of ring-buffer empty.
When the reading is completed, the buffer shall be removed from the FIFO with `spsc_ringbuff_remove_element`.

```c
if((ptr = (int *)spsc_ringbuff_get_read_buffer(rb)) != NULL)
{
   // Use the buffer for reading.
   // = *ptr;

   // Release the buffer (remove from FIFO).
   spsc_ringbuff_remove_element(rb);
}
```

# License
This code is licensed under MIT.
