#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <asprintf.h>
#include <console.h>

#include <yaffsfs.h>

#define VAL_STDIN	1
#define VAL_STDOUT	2
#define VAL_STDERR	3

FILE *stdin = (FILE *)VAL_STDIN;
FILE *stdout = (FILE *)VAL_STDOUT;
FILE *stderr = (FILE *)VAL_STDERR;

static int is_std_stream(FILE *fd)
{
	int f;
	
	f = (int)fd;
	return (f == VAL_STDIN) || (f == VAL_STDOUT) || (f == VAL_STDERR);
}

/* 
 * Mode conversion function based on NuttX
 * 
 *   Copyright (C) 2007-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

enum open_mode_e {
	MODE_NONE = 0,	/* No access mode determined */
	MODE_R,		/* "r" or "rb" open for reading */
	MODE_W,		/* "w" or "wb" open for writing, truncating or creating file */
	MODE_A,		/* "a" or "ab" open for writing, appending to file */
	MODE_RPLUS,	/* "r+", "rb+", or "r+b" open for update (reading and writing) */
	MODE_WPLUS,	/* "w+", "wb+", or "w+b"  open for update, truncating or creating file */
	MODE_APLUS,	/* "a+", "ab+", or "a+b" open for update, appending to file */
};

static int mode2oflags(const char *mode)
{
	enum open_mode_e state;
	int oflags;

	/* Verify that a mode string was provided. */
	if(!mode)
		goto errout;

	/* Parse the mode string to determine the corresponding open flags */
	state = MODE_NONE;
	oflags = 0;

	for(;*mode;mode++) {
		switch(*mode) {
			/* Open for read access ("r", "r[+]", "r[b]",  "r[b+]", or "r[+b]") */
			case 'r':
				if(state == MODE_NONE) {
					/* Open for read access */
					oflags = O_RDONLY;
					state = MODE_R;
				} else
					goto errout;
				break;

			/* Open for write access ("w", "w[+]", "w[b]",  "w[b+]", or "w[+b]") */
			case 'w':
				if(state == MODE_NONE) {
					/* Open for write access, truncating any existing file */
					oflags = O_WRONLY|O_CREAT|O_TRUNC;
					state = MODE_W;
				} else
					goto errout;
				break;

			/* Open for write/append access ("a", "a[+]", "a[b]", "a[b+]", or "a[+b]") */
			case 'a' :
				if(state == MODE_NONE) {
					/* Write to the end of the file */
					oflags = O_WRONLY|O_CREAT|O_APPEND;
					state = MODE_A;
				} else
					goto errout;
				break;

			/* Open for update access ("[r]+", "[rb]+]", "[r]+[b]", "[w]+",
			 * "[wb]+]", "[w]+[b]", "[a]+", "[ab]+]",  "[a]+[b]")
			 */
			case '+' :
				switch(state) {
					case MODE_R:
						/* Open for read/write access */
						oflags = O_RDWR;
						state = MODE_RPLUS;
						break;

					case MODE_W:
						/* Open for write read/access, truncating any existing file */
						oflags = O_RDWR|O_CREAT|O_TRUNC;
						state = MODE_WPLUS;
						break;

					case MODE_A:
						/* Read from the beginning of the file; write to the end */
						oflags = O_RDWR|O_CREAT|O_APPEND;
						state = MODE_APLUS;
						break;

					default:
						goto errout;
						break;
				}
				break;

			/* Open for binary access ("[r]b", "[r]b[+]", "[r+]b", "[w]b",
			 * "[w]b[+]", "[w+]b", "[a]b", "[a]b[+]",  "[a+]b")
			 */
			case 'b' :
				if(state != MODE_NONE) {
					/* nothing to do */
				} else
					goto errout;
			break;

			/* Unrecognized or unsupported mode */
			default:
				goto errout;
				break;
		}
	}

	return oflags;

errout:
	errno = EINVAL;
	return -1;
}


FILE *fopen(const char *path, const char *mode)
{
	int oflags;
	int *fd;
	
	oflags = mode2oflags(mode);
	if(oflags < 0)
		return NULL;
	fd = malloc(sizeof(int));
	if(!fd)
		return NULL;
	*fd = yaffs_open(path, oflags, 0666);
	if(*fd < 0) {
		free(fd);
		return NULL;
	}
	return (FILE *)fd;
}

int fclose(FILE *fd)
{
	int r;
	
	r = yaffs_close(*(int *)fd);
	free(fd);
	return r;
}

int fprintf(FILE *stream, const char *format, ...)
{
	va_list args;
	int len;
	char *outbuf;

	va_start(args, format);
	len = vasprintf(&outbuf, format, args);
	va_end(args);
	if(len < 0)
		return len;
	
	if(is_std_stream(stream)) {
		putsnonl(outbuf);
		free(outbuf);
	} else {
		int fd = *(int *)stream;
		
		len = yaffs_write(fd, outbuf, len);
		free(outbuf);
	}
	return len;
}

int fflush(FILE *stream)
{
	/* nothing to do */
	return 0;
}

char *fgets(char *s, int size, FILE *stream)
{
	return NULL; /* TODO */
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if(is_std_stream(stream))
		return 0;
	else {
		int fd = *(int *)stream;
		int r;
		
		r = yaffs_read(fd, ptr, size*nmemb);
		if(r < 0)
			return r;
		return r/size;
	}
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if(is_std_stream(stream)) {
		const char *str = ptr;
		int i;
		
		for(i=0;i<size*nmemb;i++)
			putchar(str[i]);
		return nmemb;
	} else {
		int fd = *(int *)stream;
		int r;
		
		r = yaffs_write(fd, ptr, size*nmemb);
		if(r < 0)
			return r;
		return r/size;
	}
}

int getc(FILE *stream)
{
	if(is_std_stream(stream))
		return EOF;
	else {
		int fd = *(int *)stream;
		int r;
		char c;
		
		r = yaffs_read(fd, &c, 1);
		if(r < 0)
			return EOF;
		return c;
	}
}

int fputc(int c, FILE *stream)
{
	if(is_std_stream(stream)) {
		putchar(c);
		return c;
	} else {
		int fd = *(int *)stream;
		int r;
		char c2;
		
		c2 = c;
		r = yaffs_write(fd, &c2, 1);
		if(r < 0)
			return EOF;
		return c;
	}
}

int ferror(FILE *stream)
{
	return 0;
}

int feof(FILE *stream)
{
	int fd;
	loff_t position;
	loff_t end;
	
	if(is_std_stream(stream))
		return 1;
	
	fd = *(int *)stream;
	position = yaffs_lseek(fd, 0, SEEK_CUR);
	end = yaffs_lseek(fd, 0, SEEK_END);
	yaffs_lseek(fd, position, SEEK_SET);
	return position == end;
}

FILE *freopen(const char *path, const char *mode, FILE *stream)
{
	FILE *newfd;
	
	if(is_std_stream(stream))
		return NULL; /* unsupported */
	
	yaffs_close(*(int *)stream);
	newfd = fopen(path, mode);
	if(newfd == NULL) {
		free(stream);
		return NULL;
	}
	*(int *)stream = *(int *)newfd;
	free(newfd);
	return stream;
}

int fseek(FILE *stream, long offset, int whence)
{
	int fd;
	
	if(is_std_stream(stream))
		return -1;
	fd = *(int *)stream;
	if(yaffs_lseek(fd, offset, whence) < 0)
		return -1;
	return 0;
}

long ftell(FILE *stream)
{
	int fd;
	
	if(is_std_stream(stream))
		return -1;
	fd = *(int *)stream;
	return yaffs_lseek(fd, 0, SEEK_CUR);
}
