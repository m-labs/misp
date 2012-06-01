#ifndef __MALLOC_H
#define __MALLOC_H

struct mallinfo
{
  int arena;    /* This is the total size of memory allocated
                 * for use by malloc in bytes. */
  int ordblks;  /* This is the number of free (not in use) chunks */
  int mxordblk; /* Size of the largest free (not in use) chunk */
  int uordblks; /* This is the total size of memory occupied by
                 * chunks handed out by malloc. */
  int fordblks; /* This is the total size of memory occupied
                 * by free (not in use) chunks.*/
};

struct mallinfo mallinfo(void);
void *memalign(size_t boundary, size_t size);

/* Those are non-standard */
void mm_initialize(void *heapstart, size_t heapsize);
void mm_addregion(void *heapstart, size_t heapsize);
void *zalloc(size_t size);

#endif /* __MALLOC_H */
