#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <console.h>

#include <yaffs_osglue.h>

/* TODO */

unsigned int yaffs_trace_mask;

void yaffsfs_Lock(void)
{
}

void yaffsfs_Unlock(void)
{
}

u32 yaffsfs_CurrentTime(void)
{
	return 0;
}

void yaffsfs_SetError(int err)
{
	errno = err;
}

int yaffsfs_GetLastError(void)
{
	return errno;
}

void *yaffsfs_malloc(size_t size)
{
	return NULL;
}

void yaffsfs_free(void *ptr)
{
}

void yaffs_bug_fn(const char *file_name, int line_no)
{
}


FILE *stdin;
FILE *stdout;
FILE *stderr;

int fprintf(FILE *stream, const char *format, ...)
{
	va_list args;
	int len;
	char outbuf[256];

	va_start(args, format);
	len = vscnprintf(outbuf, sizeof(outbuf), format, args);
	va_end(args);
	outbuf[len] = 0;
	putsnonl(outbuf);
	return len;
}

int fflush(FILE *stream)
{
	return 0;
}

FILE *fopen(const char *path, const char *mode)
{
	return NULL;
}

char *fgets(char *s, int size, FILE *stream)
{
	return NULL;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	const char *str = ptr;
	int i;
	
	for(i=0;i<size*nmemb;i++)
		putchar(str[i]);
	return nmemb;
}

int getc(FILE *stream)
{
	return 0;
}

int fputc(int c, FILE *stream)
{
	return 0;
}


int ferror(FILE *stream)
{
	return 0;
}

int feof(FILE *stream)
{
	return 0;
}

int fclose(FILE *fp)
{
	return 0;
}

FILE *freopen(const char *path, const char *mode, FILE *stream)
{
	return NULL;
}
