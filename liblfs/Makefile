MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-I$(MISPDIR)/liblfs/include -I$(YAFFSDIR)/direct -I$(YAFFSDIR) -DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_DEFINES_TYPES -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES -I$(LUADIR)/src
OBJECTS=lfs.o

all: liblfs.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

liblfs.a: $(OBJECTS)
	$(AR) clr liblfs.a $(OBJECTS)
	$(RANLIB) liblfs.a

%.o: %.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) liblfs.a .*~ *~
