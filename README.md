# CMCM

This is a very simple implementation of cooperative multitasking (via context switching) for ARM Cortex-M microcontrollers.

**Includes:**

* context switching between multiple tasks (tasks run to completion or yield back to the "OS")
* statically allocated stack space for tasks (configurable)
* task sleep/wake/delay
* mutexes
* queues (receiving tasks block until data is available)

**Does not include:**

* task preemption
* task priorities
* basically everything else you'd find in an RTOS

I use this as an alternative to evented, state machine based multitasking since the semantics are *almost* as easily understood and it alleviates the need for a lot of asynchronous code and state management.

## Setup

There are two configurable values that CMCM expects:

```c
#ifndef CMCM_MAX_NUM_TASKS
#define CMCM_MAX_NUM_TASKS 8
#endif

#ifndef CMCM_STACK_SIZE
#define CMCM_STACK_SIZE 2048
#endif
```

These values determine the size of the statically allocated memory devoted to task stacks.  You should tune these values for your application so as not to needlessly waste RAM.

CMCM does not automatically hook into any interrupts.  You need to explicitly instruct CMCM to perform a context switch (probably from your `PendSV` interrupt handler):

```c
void pend_sv_handler(void) {
  cmcm_context_switch();
}
```

In addition, if you choose to use CMCM's delay function, you'll need to periodically increment CMCM's internal tick counter (probably from your `SysTick` handler):

```c
void sys_tick_handler(void) {
  cmcm_tick();
}
```

The argument to `cmcm_delay(ticks)` is the number of ticks to delay the current task, so if your systick handler runs every millisecond then `cmcm_delay(100)` will put your task to sleep for 100ms.

## Tasks

Tasks are functions that can run concurrently, each with their own call stack.

API:

```c
// create a new task, immediately placing it in rotation for context switches
void cmcm_create_task(void (*handler)(void));

// interrupt the current task by triggering a context switch
void cmcm_yield(void);

// delay the current task for the given number of ticks (yields internally)
void cmcm_delay(uint32_t ticks);

// interrupt the current task and put it to sleep until further notice (removes the task from context switch rotation)
void cmcm_sleep(void);

// wake the task with the given ID (puts it back into rotation for context switching)
void cmcm_wake(int task_id);

// returns the ID of the current task
int cmcm_current_task(void);
```

Here's an example of a simple task that blinks an LED:

```c
void blink_task(void) {
  while (1) {
    led_on();
    cmcm_delay(100);
    led_off();
    cmcm_delay(100);
  }
}

int main(void) {
  // ... harware initialization, etc.

  cmcm_create_task(blink_task);
  cmcm_yield(); // passes control to CMCM

  return 0;
}
```

Tasks can loop forever or eventually return.  If a task returns then its stack space becomes available for another task to be dynamically created via `cmcm_create_task`.  Notice that you need to explicitly yield to CMCM at the end of your `main` function.  After this initial yield, control will never return to your `main` function so all remaining execution needs to be handled by tasks.

## Synchronization

**Mutexes**

Useful for preventing concurrent access to shared resources.  Nothing special here.

API:

```c
void cmcm_mutex_lock(cmcm_mutex_t *mutex);
void cmcm_mutex_unlock(cmcm_mutex_t *mutex);
```

For example, you could use mutexes to allow multiple tasks to write to the same I2C bus:

```c
static cmcm_mutex_t mutex;

void task1(void) {
  while (1) {
    cmcm_mutex_lock(&mutex);
    i2c_write(...);
    cmcm_mutex_unlock(&mutex);
    cmcm_yield();
  }
}

void task2(void) {
  while (1) {
    cmcm_mutex_lock(&mutex);
    i2c_write(...);
    cmcm_mutex_unlock(&mutex);
    cmcm_yield();
  }
}
```

**Queues**

FIFO queues for producer/consumer task synchronization.  Useful for waiting on asynchronous events (among other things).  Queues are also statically allocated and have a configurable size:

```c
#ifndef CMCM_QUEUE_SIZE
#define CMCM_QUEUE_SIZE 10
#endif
```

API:

```c
// initialize a new queue
void cmcm_queue_init(cmcm_queue_t *queue);

// place a message at the end of the queue (never blocks)
void cmcm_queue_put(cmcm_queue_t *queue, cmcm_msg_t msg);

// receive (or wait for) the next message in the queue (blocks until a message is available)
void cmcm_queue_receive(cmcm_queue_t *queue, cmcm_msg_t *msg);
```

Here's an example of a task waiting for an asynchronous event from an interrupt (say, the completion of an ADC conversion):

```c
static cmcm_queue_t queue;

void the_task(void) {
  cmcm_queue_init(&queue);

  while (1) {
    // starts an ADC conversion running and returns immediately
    adc_start_conversion();

    // wait for an ADC value off the queue
    cmcm_msg_t msg;
    cmcm_queue_receive(&queue, &msg);

    printf("value=%d\n", msg.value);
    cmcm_delay(1000);
  }
}

// here's the contrived interrupt handler
void adc_conversion_complete(void) {
  cmcm_msg_t msg;
  msg.value = adc_read();
  cmcm_queue_put(&queue, msg);
}
```
