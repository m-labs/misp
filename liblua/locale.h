#ifndef __DUMMY_LOCALE_H
#define __DUMMY_LOCALE_H

static inline char mygetlocaledecpoint(void)
{
	return '.';
}

#define getlocaledecpoint mygetlocaledecpoint

#endif /* __DUMMY_LOCALE_H */

