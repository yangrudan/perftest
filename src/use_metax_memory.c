#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mcr/mc_runtime_api.h>
#include "use_metax_memory.h"
#include "perftest_parameters.h"

#define METAX_CHECK(stmt)			\
	do {					\
	mcError_t result = (stmt);		\
	ASSERT(mcSuccess == result);		\
} while (0)

#define ACCEL_PAGE_SIZE (64 * 1024)


struct metax_memory_ctx {
	struct memory_ctx base;
	int device_id;
};


static int init_metax(int device_id) {
	int deviceCount = 0;
	mcError_t error = mcGetDeviceCount(&deviceCount);

	if (error != mcSuccess) {
		printf("mcDeviceGetCount() returned %d\n", error);
		return FAILURE;
	}

	if (device_id >= deviceCount) {
		printf("Requested metax device %d but found only %d device(s)\n",
               device_id, deviceCount);
		return FAILURE;
	}

	METAX_CHECK(mcSetDevice(device_id));

	mcDeviceProp_t prop = {0};
	METAX_CHECK(mcGetDeviceProperties(&prop, device_id));
	printf("Using metax Device with ID: %d, Name: %s, PCI Bus ID: 0x%x, metax Arch: %s\n",
	       device_id, prop.name, prop.pciBusID, prop.mxArchName);

	return SUCCESS;
}

int metax_memory_init(struct memory_ctx *ctx) {
	struct metax_memory_ctx *metax_ctx = container_of(ctx, struct metax_memory_ctx, base);

	if (init_metax(metax_ctx->device_id)) {
		fprintf(stderr, "Couldn't initialize metax device\n");
		return FAILURE;
	}
	return SUCCESS;
}

int metax_memory_destroy(struct memory_ctx *ctx) {
	struct metax_memory_ctx *metax_ctx = container_of(ctx, struct metax_memory_ctx, base);

	free(metax_ctx);
	return SUCCESS;
}

int metax_memory_allocate_buffer(struct memory_ctx *ctx, int alignment, uint64_t size, int *dmabuf_fd,
				uint64_t *dmabuf_offset, void **addr, bool *can_init) {
	void *d_A;
	mcError_t error;
	size_t buf_size = (size + ACCEL_PAGE_SIZE - 1) & ~(ACCEL_PAGE_SIZE - 1);

	error = mcMalloc(&d_A, buf_size);
	if (error != mcSuccess) {
		printf("mcMalloc error=%d\n", error);
		return FAILURE;
	}

	printf("allocated %lu bytes of GPU buffer at %p\n", (unsigned long)buf_size, d_A);
	*addr = d_A;
	*can_init = false;
	return SUCCESS;
}

int metax_memory_free_buffer(struct memory_ctx *ctx, int dmabuf_fd, void *addr, uint64_t size) {
	printf("deallocating GPU buffer %p\n", addr);
	mcFree(addr);
	return SUCCESS;
}

bool metax_memory_supported() {
	return true;
}

struct memory_ctx *metax_memory_create(struct perftest_parameters *params) {
	struct metax_memory_ctx *ctx;

	ALLOCATE(ctx, struct metax_memory_ctx, 1);
	ctx->base.init = metax_memory_init;
	ctx->base.destroy = metax_memory_destroy;
	ctx->base.allocate_buffer = metax_memory_allocate_buffer;
	ctx->base.free_buffer = metax_memory_free_buffer;
	ctx->base.copy_host_to_buffer = memcpy;
	ctx->base.copy_buffer_to_host = memcpy;
	ctx->base.copy_buffer_to_buffer = memcpy;
	ctx->device_id = params->metax_device_id;

	return &ctx->base;
}