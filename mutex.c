#include "cmcm.h"
#include "mutex.h"

void cmcm_mutex_lock(cmcm_mutex_t *mutex) {
  // block untli the lock is aquired
  while (1) {
    // critical section for reading shared memory
    cmcm_disable_interrupts();
    uint8_t locked = mutex->locked;
    cmcm_enable_interrupts();

    if (!locked) {
      break;
    } else {
      cmcm_yield();
    }
  }

  // critical section for writing shared memory
  cmcm_disable_interrupts();
  mutex->locked = 1;
  cmcm_enable_interrupts();
}

void cmcm_mutex_unlock(cmcm_mutex_t *mutex) {
  cmcm_disable_interrupts();
  mutex->locked = 0;
  cmcm_enable_interrupts();
}
