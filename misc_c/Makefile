include $(FSLCONFDIR)/default.mk

PROJNAME = misc_c
LIBS     = -lfsl-cprob
SCRIPTS  = tmpnam
XFILES   = whirlgif hist2prob morph_kernel ptoz ztop

all: $(XFILES)

hist2prob: hist2prob.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

morph_kernel: morph_kernel.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

whirlgif: gifdecod.c gifencod.c whirlgif.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

ptoz: ptoz.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

ztop: ztop.cc
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
