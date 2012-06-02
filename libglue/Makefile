MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-I$(YAFFSDIR)/direct -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_DEFINES_TYPES -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES
OBJECTS=yaffs.o file.o getenv.o

all: libglue.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libglue.a: $(OBJECTS)
	$(AR) clr libglue.a $(OBJECTS)
	$(RANLIB) libglue.a

%.o: %.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libglue.a .*~ *~
