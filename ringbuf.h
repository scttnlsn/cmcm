#ifndef CMCM_RINGBUF_H
#define CMCM_RINGBUF_H

#include <stdint.h>

typedef struct {
  uint16_t capacity;
  uint16_t count;
  uint8_t size;
  void *buffer;
  void *head;
  void *tail;
} cmcm_ringbuf_t;

void cmcm_ringbuf_init(cmcm_ringbuf_t *ringbuf, void *buffer, uint16_t capacity, uint8_t size);
uint8_t cmcm_ringbuf_push(cmcm_ringbuf_t *ringbuf, const void *data);
uint8_t cmcm_ringbuf_pop(cmcm_ringbuf_t *ringbuf, void *data);

#endif
