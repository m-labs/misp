MISPDIR=..
include $(MISPDIR)/common.mak

CXXFLAGS+=-I$(MISPDIR)/libagl/include -I$(MISPDIR)/libm/include
OBJECTS=agg.o

all: libagl.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libagl.a: $(OBJECTS)
	$(AR) clr libagl.a $(OBJECTS)
	$(RANLIB) libagl.a

%.o: %.cpp
	$(compilexx-dep)

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libagl.a .*~ *~
