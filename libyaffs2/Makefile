MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-include $(M2DIR)/software/include/base/errno.h -include $(YAFFSDIR)/direct/yportenv.h -I$(YAFFSDIR)/direct -I$(YAFFSDIR)
CFLAGS+=-DCONFIG_YAFFS_DIRECT -DCONFIG_YAFFS_DEFINES_TYPES -DCONFIG_YAFFS_PROVIDE_DEFS -DCONFIG_YAFFSFS_PROVIDE_VALUES

OBJECTS=yaffs_ecc.o yaffs_guts.o yaffs_packedtags1.o yaffs_tagscompat.o yaffs_packedtags2.o yaffs_nand.o yaffs_checkptrw.o yaffs_nameval.o yaffs_attribs.o yaffs_allocator.o yaffs_bitmap.o yaffs_yaffs1.o yaffs_yaffs2.o yaffs_verify.o yaffs_summary.o
OBJECTS+=yaffs_hweight.o yaffs_qsort.o yaffsfs.o

all: libyaffs2.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libyaffs2.a: $(OBJECTS)
	$(AR) clr libyaffs2.a $(OBJECTS)
	$(RANLIB) libyaffs2.a

%.o: $(YAFFSDIR)/%.c
	$(compile-dep)

%.o: $(YAFFSDIR)/direct/%.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libyaffs2.a .*~ *~
