MISPDIR=.
include $(MISPDIR)/common.mak

OBJECTS=isr.o luainit.o agg_test.o main.o
OURLIBS=m mm yaffs2 glue lua lfs agl

INCFLAGS=-I$(MISPDIR)/libm/include -I$(MISPDIR)/libmm/include -I$(MISPDIR)/libglue/include -I$(LUADIR)/src -I$(MISPDIR)/liblfs/include -I$(MISPDIR)/libagl/include
CFLAGS+=$(INCFLAGS)
CXXFLAGS+=$(INCFLAGS)

all: misp.bin

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	chmod -x $@

misp.elf: $(OBJECTS) libs

%.elf:
	$(LD) $(LDFLAGS) \
		-T $(M2DIR)/software/libbase/linker-sdram.ld \
		-N -o $@ \
		$(M2DIR)/software/libbase/crt0.o \
		$(OBJECTS) \
		-L$(M2DIR)/software/libbase \
		-L$(M2DIR)/software/libcompiler-rt \
		$(addprefix -L$(MISPDIR)/lib,$(OURLIBS)) \
		--start-group -lbase -lcompiler-rt $(addprefix -l,$(OURLIBS)) --end-group
	chmod -x $@

%.o: %.cpp
	$(compilexx-dep)

%.o: %.c
	$(compile-dep)


libs:
	set -e; \
	for lib in $(OURLIBS); do \
		make -C $(MISPDIR)/lib$$lib; \
	done

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) misp.elf misp.bin .*~ *~
	for lib in $(OURLIBS); do \
		make -C $(MISPDIR)/lib$$lib clean; \
	done

netboot: misp.bin
	cp misp.bin /srv/tftp/boot.bin

.PHONY: clean libs netboot
