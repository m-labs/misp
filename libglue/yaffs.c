#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <console.h>

#include <hw/mem.h>

#include <yaffsfs.h>

/*
 * YAFFS callbacks
 */

unsigned int yaffs_trace_mask;

void yaffsfs_Lock(void)
{
	/* nothing to do */
}

void yaffsfs_Unlock(void)
{
	/* nothing to do */
}

u32 yaffsfs_CurrentTime(void)
{
	return 0;
}

void yaffsfs_SetError(int err)
{
	errno = -err;
}

int yaffsfs_GetLastError(void)
{
	return -errno;
}

void *yaffsfs_malloc(size_t size)
{
	return malloc(size);
}

void yaffsfs_free(void *ptr)
{
	free(ptr);
}

void yaffs_bug_fn(const char *file_name, int line_no)
{
	printf("YAFFS BUG in %s line %d\n", file_name, line_no);
}

/*
 * YAFFS init and flash access
 */