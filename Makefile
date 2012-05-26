MISPDIR=.
include $(MISPDIR)/common.mak

OBJECTS=crt0.o isr.o main.o

all: misp.bin

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	chmod -x $@

misp.elf: linker.ld $(OBJECTS) libs

%.elf:
	$(LD) $(LDFLAGS) -T $< -N -o $@ $(OBJECTS) -L$(M2DIR)/software/libbase -lbase
	chmod -x $@

libs:
	make -C $(MISPDIR)/libm
	make -C $(MISPDIR)/libglue
	make -C $(MISPDIR)/liblua

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) misp.elf misp.bin .*~ *~

.PHONY: clean libs flash
