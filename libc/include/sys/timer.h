#ifndef _TIMER_H
#define _TIMER_H 1

#include <sys/io.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_timer(uint32_t frequency);
uint32_t get_tick();

#ifdef __cplusplus
}
#endif

#endif