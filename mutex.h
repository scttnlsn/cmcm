#ifndef CMCM_MUTEX_H
#define CMCM_MUTEX_H

#include <stdint.h>

typedef struct {
  uint8_t locked;
} cmcm_mutex_t;

void cmcm_mutex_lock(cmcm_mutex_t *mutex);
void cmcm_mutex_unlock(cmcm_mutex_t *mutex);

#endif
