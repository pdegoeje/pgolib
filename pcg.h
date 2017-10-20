#ifndef PCG_H
#define PCG_H

#include <stdint.h>

uint32_t pcg_next(uint64_t *state);
uint64_t pcg_uniform(uint64_t *state, uint64_t bound);
void pcg_seed(uint64_t *state) ;

#endif
