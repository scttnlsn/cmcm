#include <string.h>

#include "ringbuf.h"

void cmcm_ringbuf_init(cmcm_ringbuf_t *ringbuf, void *buffer, uint16_t capacity, uint8_t size) {
  ringbuf->capacity = capacity;
  ringbuf->count = 0;
  ringbuf->size = size;
  ringbuf->buffer = buffer;
  ringbuf->head = buffer;
  ringbuf->tail = buffer;
}

uint8_t cmcm_ringbuf_push(cmcm_ringbuf_t *ringbuf, const void *data) {
  if (ringbuf->count == ringbuf->capacity) {
    // full
    return 0;
  }

  memcpy(ringbuf->head, data, ringbuf->size);

  ringbuf->head += ringbuf->size;
  if (ringbuf->head == ringbuf->buffer + (ringbuf->size * ringbuf->capacity)) {
    // wrap around
    ringbuf->head = ringbuf->buffer;
  }

  ringbuf->count++;
  return 1;
}

uint8_t cmcm_ringbuf_pop(cmcm_ringbuf_t *ringbuf, void *data) {
  if (ringbuf->count == 0) {
    // empty
    return 0;
  }

  memcpy(data, ringbuf->tail, ringbuf->size);

  ringbuf->tail += ringbuf->size;
  if (ringbuf->tail == ringbuf->buffer + (ringbuf->size * ringbuf->capacity)) {
    // wrap around
    ringbuf->tail = ringbuf->buffer;
  }

  ringbuf->count--;
  return 1;
}
