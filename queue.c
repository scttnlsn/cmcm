#include "cmcm.h"
#include "mutex.h"
#include "queue.h"
#include "ringbuf.h"

void cmcm_queue_init(cmcm_queue_t *queue) {
  cmcm_ringbuf_init(&queue->messages_ring, queue->messages, sizeof(queue->messages) / sizeof(cmcm_msg_t), sizeof(cmcm_msg_t));
  cmcm_ringbuf_init(&queue->waiters_ring, queue->waiters, sizeof(queue->waiters) / sizeof(int), sizeof(int));
}

void cmcm_queue_receive(cmcm_queue_t *queue, cmcm_msg_t *msg) {
  cmcm_mutex_lock(&queue->mutex);
  uint8_t available = cmcm_ringbuf_pop(&queue->messages_ring, msg);
  cmcm_mutex_unlock(&queue->mutex);

  if (available) {
    // message was ready, no need to wait
    return;
  }

  // put task to sleep until message becomes available
  cmcm_mutex_lock(&queue->mutex);
  int task_id = cmcm_current_task();
  cmcm_ringbuf_push(&queue->waiters_ring, &task_id);
  cmcm_mutex_unlock(&queue->mutex);
  cmcm_sleep();

  cmcm_mutex_lock(&queue->mutex);
  available = cmcm_ringbuf_pop(&queue->messages_ring, msg);
  cmcm_mutex_unlock(&queue->mutex);

  if (!available) {
    // error: nothing available
  }
}

void cmcm_queue_put(cmcm_queue_t *queue, cmcm_msg_t msg) {
  msg.sender = cmcm_current_task();

  cmcm_mutex_lock(&queue->mutex);
  uint8_t ok = cmcm_ringbuf_push(&queue->messages_ring, &msg);
  cmcm_mutex_unlock(&queue->mutex);

  if (!ok) {
    // error: ringbuf full
    return;
  }

  int task_id;
  cmcm_mutex_lock(&queue->mutex);
  uint8_t available = cmcm_ringbuf_pop(&queue->waiters_ring, &task_id);
  cmcm_mutex_unlock(&queue->mutex);

  if (available) {
    // there's a task waiting on this message
    cmcm_wake(task_id);
  }
}
