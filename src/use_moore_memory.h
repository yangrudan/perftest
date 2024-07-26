#ifndef USE_MOORE_MEMORY_H
#define USE_MOORE_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "memory.h"

struct perftest_parameters;

bool moore_memory_supported();

struct memory_ctx *moore_memory_create(struct perftest_parameters *params);


#ifndef HAVE_MUSA

inline bool moore_memory_supported() {
 	return false;
 }

 inline struct memory_ctx *moore_memory_create(struct perftest_parameters *params) {
 	return NULL;
}

// #else

// bool moore_memory_supported();

// struct memory_ctx *moore_memory_create(struct perftest_parameters *params);
#endif

#endif /* USE_MOORE_MEMORY_H */
