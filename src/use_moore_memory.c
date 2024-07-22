#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <musa.h>
#include <musa_runtime.h>
#include "use_moore_memory.h"
#include "perftest_parameters.h"

#define MOORE_CHECK(stmt)			\
	do {					\
	musaError_t result = (stmt);		\
	ASSERT(musaSuccess == result);		\
} while (0)

#define ACCEL_PAGE_SIZE (64 * 1024)


struct moore_memory_ctx {
	struct memory_ctx base;
	int device_id;
};

static MUdevice muDevice;
static MUcontext muContext;

static int init_moore(int device_id) {
    MUdevice mu_device;

    printf("initializing MUSA\n");
	error = muInit(0);
	if (error != musaSuccess)
	{
		printf("muInit() failed, %s\n", musaGetErrorString(error));
		exit(1);
	}

	int deviceCount = 0;
	musaError_t error = muDeviceGetCount(&deviceCount);

	if (error != musaSuccess) {
		printf("muDeviceGetCount() returned %d\n", error);
		return FAILURE;
	}

	if (device_id >= deviceCount) {
		printf("Requested Moore device %d but found only %d device(s)\n",
               device_id, deviceCount);
		return FAILURE;
	}

	MOORE_CHECK(muDeviceGet(&muDevice, musa_device_id));

	struct musaDeviceProp prop = {0};
	MOORE_CHECK(musaGetDeviceProperties(&prop, device_id));
	printf("Using moore Device with ID: %d, Name: %s, PCI Bus ID: 0x%x\n",
	       device_id, prop.name, prop.pciBusID);

    printf("creating MUSA Ctx\n");

	/* Create context */
	error = muCtxCreate(&muContext, MU_CTX_MAP_HOST, muDevice);
	if (error != musaSuccess)
	{
		printf("muCtxCreate() error=%d\n", error);
		return 1;
	}

	printf("making it the current MUSA Ctx\n");
	error = muCtxSetCurrent(muContext);
	if (error != musaSuccess)
	{
		printf("muCtxSetCurrent() error=%d\n", error);
		return 1;
	}

	return SUCCESS;
}

int moore_memory_init(struct memory_ctx *ctx) {
	struct moore_memory_ctx *moore_ctx = container_of(ctx, struct moore_memory_ctx, base);

	if (init_moore(moore_ctx->device_id)) {
		fprintf(stderr, "Couldn't initialize moore device\n");
		return FAILURE;
	}
    else {
        fprintf(stdout, "Initialize moore device ok!!!!!!!\n");
    }
	return SUCCESS;
}

int moore_memory_destroy(struct memory_ctx *ctx) {
    printf("destroying current MUSA Ctx\n");
	MUCHECK(muCtxDestroy(muContext));
    
	struct moore_memory_ctx *moore_ctx = container_of(ctx, struct moore_memory_ctx, base);

	free(moore_ctx);
	return SUCCESS;
}

int moore_memory_allocate_buffer(struct memory_ctx *ctx, int alignment, uint64_t size, int *dmabuf_fd,
				uint64_t *dmabuf_offset, void **addr, bool *can_init) {
	void *d_A;
	musaError_t error;
	size_t buf_size = (size + ACCEL_PAGE_SIZE - 1) & ~(ACCEL_PAGE_SIZE - 1);

	error = musaMalloc(&d_A, buf_size);
	if (error != musaSuccess) {
		printf("musaMalloc error=%d\n", error);
		return FAILURE;
	}

	printf("allocated %lu bytes of GPU buffer at %p\n", (unsigned long)buf_size, d_A);
	*addr = d_A;
	*can_init = false;
	return SUCCESS;
}

int moore_memory_free_buffer(struct memory_ctx *ctx, int dmabuf_fd, void *addr, uint64_t size) {
	printf("deallocating GPU buffer %p\n", addr);
	musaFree(addr);
	return SUCCESS;
}

bool moore_memory_supported() {
	return true;
}

struct memory_ctx *moore_memory_create(struct perftest_parameters *params) {
	struct moore_memory_ctx *ctx;

	ALLOCATE(ctx, struct moore_memory_ctx, 1);
	ctx->base.init = moore_memory_init;
	ctx->base.destroy = moore_memory_destroy;
	ctx->base.allocate_buffer = moore_memory_allocate_buffer;
	ctx->base.free_buffer = moore_memory_free_buffer;
	ctx->base.copy_host_to_buffer = memcpy;
	ctx->base.copy_buffer_to_host = memcpy;
	ctx->base.copy_buffer_to_buffer = memcpy;
	ctx->device_id = params->moore_device_id;

	return &ctx->base;
}