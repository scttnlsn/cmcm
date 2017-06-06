#ifndef CMCM_QUEUE_H
#define CMCM_QUEUE_H

#include <stdint.h>

#include "mutex.h"
#include "ringbuf.h"

#ifndef CMCM_QUEUE_SIZE
#define CMCM_QUEUE_SIZE 10
#endif

typedef struct {
  int sender;
  uint32_t value;
} cmcm_msg_t;

typedef struct {
  cmcm_msg_t messages[CMCM_QUEUE_SIZE];
  cmcm_ringbuf_t messages_ring;

  // IDs of tasks waiting on data
  int waiters[CMCM_QUEUE_SIZE];
  cmcm_ringbuf_t waiters_ring;

  cmcm_mutex_t mutex;
} cmcm_queue_t;

void cmcm_queue_init(cmcm_queue_t *queue);
void cmcm_queue_receive(cmcm_queue_t *queue, cmcm_msg_t *msg);
void cmcm_queue_put(cmcm_queue_t *queue, cmcm_msg_t msg);

#endif
