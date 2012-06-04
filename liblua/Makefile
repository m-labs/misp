MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-I. -DLUA_IEEEENDIAN=1 -Dstrcoll=strcmp

OBJECTS=lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o lundump.o lvm.o lzio.o lbaselib.o lbitlib.o lcorolib.o ldblib.o lmathlib.o lstrlib.o ltablib.o loadlib.o lauxlib.o

all: liblua.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

liblua.a: $(OBJECTS)
	$(AR) clr liblua.a $(OBJECTS)
	$(RANLIB) liblua.a

%.o: $(LUADIR)/src/%.c
	$(compile-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) liblua.a .*~ *~
