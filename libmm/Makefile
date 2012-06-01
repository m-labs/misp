MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-I$(MISPDIR)/libmm/include
OBJECTS=mm_addfreechunk.o mm_free.o mm_mallinfo.o mm_memalign.o mm_shrinkchunk.o mm_zalloc.o mm_calloc.o mm_initialize.o mm_malloc.o mm_realloc.o mm_size2ndx.o

all: libmm.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libmm.a: $(OBJECTS)
	$(AR) clr libmm.a $(OBJECTS)
	$(RANLIB) libmm.a

%.o: %.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libmm.a .*~ *~
