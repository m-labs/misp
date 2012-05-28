MISPDIR=.
include $(MISPDIR)/common.mak

OBJECTS=crt0.o isr.o main.o

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
		-L$(MISPDIR)/libm \
		-L$(MISPDIR)/libglue \
		-L$(MISPDIR)/liblua \
		--start-group -lbase -lcompiler_rt -lm -lglue -llua --end-group
	chmod -x $@

libs:
	make -C $(MISPDIR)/libm
	make -C $(MISPDIR)/libglue
	make -C $(MISPDIR)/liblua

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) misp.elf misp.bin .*~ *~
	make -C $(MISPDIR)/libm clean
	make -C $(MISPDIR)/libglue clean
	make -C $(MISPDIR)/liblua clean

.PHONY: clean libs flash
