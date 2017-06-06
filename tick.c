#include "tick.h"

#define CMCM_TICK_MAX 0xFFFFFFFF // 32-bit

uint32_t counter;

void cmcm_tick(void) {
  counter++;
}

uint32_t cmcm_tick_get(void) {
  return counter;
}

uint32_t cmcm_tick_since(uint32_t since) {
  uint32_t now = cmcm_tick_get();

  if (now >= since) {
    return now - since;
  } else {
    // counter overflow
    return now + (CMCM_TICK_MAX - since);
  }
}
