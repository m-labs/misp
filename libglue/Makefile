MISPDIR=..
include $(MISPDIR)/common.mak

OBJECTS=file.o getenv.o

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
