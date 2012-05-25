#include <stdio.h>

/* TODO */

FILE *stdin;
FILE *stdout;
FILE *stderr;

int fprintf(FILE *stream, const char *format, ...)
{
	return 0;
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
	return 0;
}

int fclose(FILE *fp)
{
	return 0;
}
