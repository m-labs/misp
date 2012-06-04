MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-Iinclude -I$(FTDIR)/include -include $(FTDIR)/include/freetype/internal/internal.h

FTSRC=$(FTDIR)/src

OBJECTS=ftadvanc.o ftcalc.o ftdbgmem.o ftgloadr.o ftobjs.o ftoutln.o ftrfork.o ftsnames.o ftstream.o fttrigon.o ftutil.o
OBJECTS+=ftbbox.o ftfstype.o ftgasp.o ftglyph.o ftgxval.o ftlcdfil.o ftmm.o ftotval.o ftstroke.o ftbitmap.o ftsynth.o
OBJECTS+=ftsystem.o ftdebug.o ftinit.o
%.o: $(FTSRC)/base/%.c
	$(compile-dep)

OBJECTS+=ttdriver.o ttgload.o ttgxvar.o ttinterp.o ttobjs.o ttpic.o ttpload.o
%.o: $(FTSRC)/truetype/%.c
	$(compile-dep)

OBJECTS+=afangles.o afcjk.o afdummy.o afglobal.o afhints.o afindic.o aflatin.o afloader.o afmodule.o afpic.o afwarp.o
%.o: $(FTSRC)/autofit/%.c
	$(compile-dep)

OBJECTS+=ftraster.o ftrend1.o rastpic.o
%.o: $(FTSRC)/raster/%.c
	$(compile-dep)

OBJECTS+=ftgrays.o ftsmooth.o ftspic.o
%.o: $(FTSRC)/smooth/%.c
	$(compile-dep)

OBJECTS+=ftcbasic.o ftccache.o ftccmap.o ftcglyph.o ftcimage.o ftcmanag.o ftcmru.o ftcsbits.o
%.o: $(FTSRC)/cache/%.c
	$(compile-dep)

all: libfreetype.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libfreetype.a: $(OBJECTS)
	$(AR) clr libfreetype.a $(OBJECTS)
	$(RANLIB) libfreetype.a

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libfreetype.a .*~ *~
