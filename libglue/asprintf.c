#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <asprintf.h>

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	va_list aq;
	int alloclen;
	int len;
	char *newbuf;
	
	alloclen = 256;
	*strp = malloc(alloclen);
	if(!*strp)
		return -1;

	while(1) {
		va_copy(aq, ap);
		len = vscnprintf(*strp, alloclen, fmt, aq);
		va_end(aq);
		if(len < 0) {
			free(*strp);
			return len;
		}
		if(len < alloclen)
			return len;
		alloclen *= 2;
		newbuf = realloc(*strp, alloclen);
		if(!newbuf) {
			free(*strp);
			return -1;
		}
		*strp = newbuf;
	}
}

int asprintf(char **strp, const char *fmt, ...)
{
	va_list args;
	int len;

	va_start(args, fmt);
	len = vasprintf(strp, fmt, args);
	va_end(args);
	return len;
}
