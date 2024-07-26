#ifndef USE_KUNLUN_MEMORY_H
#define USE_KUNLUN_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "config.h"

struct perftest_parameters;

bool kunlun_memory_supported();

struct memory_ctx *kunlun_memory_create(struct perftest_parameters *params);

#ifndef HAVE_KUNLUN

inline bool kunlun_memory_supported() {
	return false;
}

inline struct memory_ctx *kunlun_memory_create(struct perftest_parameters *params) {
	return NULL;
}

#endif
#endif /* USE_KUNLUN_MEMORY_H */
