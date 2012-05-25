MISPDIR=..
include $(MISPDIR)/common.mak

CFLAGS+=-D_IEEE_LIBM -D_REENTRANT

OBJECTS= \
e_acos.o   e_gamma.o     e_log10.o      k_cos.o       s_ceil.o      s_frexp.o        s_modf.o         s_tanh.o   w_fmod.o     w_lgamma_r.o \
e_acosh.o  e_gamma_r.o   e_log.o        k_rem_pio2.o  s_copysign.o  s_ilogb.o        s_nextafter.o    w_acos.o   w_gamma.o    w_log10.o \
e_asin.o   e_hypot.o     e_pow.o        k_sin.o       s_cos.o       s_isnan.o        s_rint.o         w_acosh.o  w_gamma_r.o  w_log.o \
e_atan2.o  e_j0.o        e_remainder.o  k_standard.o  s_erf.o       s_ldexp.o        s_scalbn.o       w_asin.o   w_hypot.o    w_pow.o \
e_atanh.o  e_j1.o        e_rem_pio2.o   k_tan.o       s_expm1.o     s_lib_version.o  s_signgam.o      w_atan2.o  w_j0.o       w_remainder.o \
e_cosh.o   e_jn.o        e_scalb.o      s_asinh.o     s_fabs.o      s_log1p.o        s_significand.o  w_atanh.o  w_j1.o       w_scalb.o \
e_exp.o    e_lgamma.o    e_sinh.o       s_atan.o      s_finite.o    s_logb.o         s_sin.o          w_cosh.o   w_jn.o       w_sinh.o \
e_fmod.o   e_lgamma_r.o  e_sqrt.o       s_cbrt.o      s_floor.o     s_matherr.o      s_tan.o          w_exp.o    w_lgamma.o   w_sqrt.o


all: libm.a

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d)

libm.a: $(OBJECTS)
	$(AR) clr libm.a $(OBJECTS)
	$(RANLIB) libm.a

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) libm.a .*~ *~
