MISPDIR=.
include $(MISPDIR)/common.mak

OBJECTS=crt0.o isr.o luainit.o main.o
OURLIBS=m yaffs2 glue lua

CFLAGS+=-I$(MISPDIR)/libm/include -I$(LUADIR)/src

all: misp.bin

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	chmod -x $@

misp.elf: linker.ld $(OBJECTS) libs

%.elf:
	$(LD) $(LDFLAGS) -T $< -N -o $@ $(OBJECTS) \
		-L$(M2DIR)/software/libbase \
		-L$(CRTDIR) \
		$(addprefix -L$(MISPDIR)/lib,$(OURLIBS)) \
		--start-group -lbase -lcompiler_rt $(addprefix -l,$(OURLIBS)) --end-group
	chmod -x $@

%.o: %.c
	$(compile-dep)

libs:
	for lib in $(OURLIBS); do \
		make -C $(MISPDIR)/lib$$lib; \
	done

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) misp.elf misp.bin .*~ *~
	for lib in $(OURLIBS); do \
		make -C $(MISPDIR)/lib$$lib clean; \
	done

.PHONY: clean libs flash
