#ifndef USE_METAX_MEMORY_H
#define USE_METAX_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "config.h"

struct perftest_parameters;

bool metax_memory_supported();

struct memory_ctx *metax_memory_create(struct perftest_parameters *params);


#ifndef HAVE_MACA

inline bool metax_memory_supported() {
	return false;
}

inline struct memory_ctx *metax_memory_create(struct perftest_parameters *params) {
	return NULL;
}

#endif

#endif /* USE_METAX_MEMORY_H */
