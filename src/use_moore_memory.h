#ifndef USE_MOORE_MEMORY_H
#define USE_MOORE_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "memory.h"

struct perftest_parameters;

bool moore_memory_supported();

struct memory_ctx *moore_memory_create(struct perftest_parameters *params);



#endif /* USE_MOORE_MEMORY_H */
