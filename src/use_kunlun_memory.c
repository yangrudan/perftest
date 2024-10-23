#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include "runtime.h"
#include "use_kunlun_memory.h"
#include "perftest_parameters.h"


#define ACCEL_PAGE_SIZE (64 * 1024)

uint64_t devinfo;

struct kunlun_memory_ctx {
	struct memory_ctx base;
	int device_id;
};


static int init_kunlun(int device_id) {
	int deviceCount = 0;
	int error = xpu_device_count(&deviceCount);

	if (error != XPU_SUCCESS) {
		printf("xpu_device_count() returned %d\n", error);
		return FAILURE;
	}

	if (device_id >= deviceCount) {
		printf("Requested kunlun device %d but found only %d device(s)\n",
               device_id, deviceCount);
		return FAILURE;
	}

	xpu_set_device(device_id);
    xpu_current_device(&device_id);
    xpu_device_get_attr(&devinfo, XPUATTR_DEVID, device_id);

	return SUCCESS;
}

int kunlun_memory_init(struct memory_ctx *ctx) {
	struct kunlun_memory_ctx *kunlun_ctx = container_of(ctx, struct kunlun_memory_ctx, base);

	if (init_kunlun(kunlun_ctx->device_id)) {
		fprintf(stderr, "Couldn't initialize kunlun device\n");
		return FAILURE;
	}
    else {
        fprintf(stdout, "Initialize kunlun device ok!!!!!!!\n");
    }
	return SUCCESS;
}

int kunlun_memory_destroy(struct memory_ctx *ctx) {
	struct kunlun_memory_ctx *kunlun_ctx = container_of(ctx, struct kunlun_memory_ctx, base);

	free(kunlun_ctx);
	return SUCCESS;
}

int kunlun_memory_allocate_buffer(struct memory_ctx *ctx, int alignment, uint64_t size, int *dmabuf_fd,
				uint64_t *dmabuf_offset, void **addr, bool *can_init) {
	void *d_A;
	int error;
	size_t buf_size = (size + ACCEL_PAGE_SIZE - 1) & ~(ACCEL_PAGE_SIZE - 1);

	error = xpu_malloc(&d_A, buf_size, XPU_MEM_MAIN);
	if (error != XPU_SUCCESS) {
		printf("xpu_malloc error=%d\n", error);
		return FAILURE;
	}

    int fd = open("/dev/xdrdrv", O_RDWR);
    if (fd < 0)
    {
        printf("open XDR failed");
    }

    uint64_t mydst = (uint64_t)(uintptr_t)d_A;
    //off_t offset = 0x0000000090001000;
    off_t offset = (devinfo << 60) | mydst;
    void* vaddr = mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);

	printf("allocated %lu bytes of GPU buffer at %p\n", (unsigned long)buf_size, d_A);
	*addr = vaddr;
	*can_init = false;
	return SUCCESS;
}

int kunlun_memory_free_buffer(struct memory_ctx *ctx, int dmabuf_fd, void *addr, uint64_t size) {
	printf("deallocating GPU buffer %p\n", addr);
	xpu_free(addr);
	return SUCCESS;
}

bool kunlun_memory_supported() {
	return true;
}

struct memory_ctx *kunlun_memory_create(struct perftest_parameters *params) {
	struct kunlun_memory_ctx *ctx;

	ALLOCATE(ctx, struct kunlun_memory_ctx, 1);
	ctx->base.init = kunlun_memory_init;
	ctx->base.destroy = kunlun_memory_destroy;
	ctx->base.allocate_buffer = kunlun_memory_allocate_buffer;
	ctx->base.free_buffer = kunlun_memory_free_buffer;
	ctx->base.copy_host_to_buffer = memcpy;
	ctx->base.copy_buffer_to_host = memcpy;
	ctx->base.copy_buffer_to_buffer = memcpy;
	ctx->device_id = params->kunlun_device_id;

	return &ctx->base;
}
