MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-I$(YAFFSDIR)/direct -I$(YAFFSDIR) -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_DEFINES_TYPES -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES -I$(MISPDIR)/libglue/include
OBJECTS=yaffs.o asprintf.o file.o getenv.o cppmm.o

all: libglue.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libglue.a: $(OBJECTS)
	$(AR) clr libglue.a $(OBJECTS)
	$(RANLIB) libglue.a

%.o: %.cpp
	$(compilexx-dep)

%.o: %.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libglue.a .*~ *~
