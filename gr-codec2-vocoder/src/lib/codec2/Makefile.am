AM_CFLAGS = -I../src -fPIC -Wall -O3
AUTOMAKE_OPTS = gnu
NAME = codec2
AM_CPPFLAGS = $(AM_CFLAGS)

D=codebook

# lsp quantisers

CODEBOOKS= \
  $D/lsp1.txt \
  $D/lsp2.txt \
  $D/lsp3.txt \
  $D/lsp4.txt \
  $D/lsp5.txt \
  $D/lsp6.txt \
  $D/lsp7.txt \
  $D/lsp8.txt \
  $D/lsp9.txt \
  $D/lsp10.txt

# lspd quantisers

CODEBOOKSD= \
  $D/dlsp1.txt \
  $D/dlsp2.txt \
  $D/dlsp3.txt \
  $D/dlsp4.txt \
  $D/dlsp5.txt \
  $D/dlsp6.txt \
  $D/dlsp7.txt \
  $D/dlsp8.txt \
  $D/dlsp9.txt \
  $D/dlsp10.txt

# lspd VQ quantisers

CODEBOOKSDVQ= \
  $D/dlsp1.txt \
  $D/dlsp2.txt \
  $D/dlsp3.txt \
  $D/dlsp4.txt \
  $D/dlsp5.txt 

noinst_PROGRAMS = generate_codebook

codebook.$(OBJEXT): codebook.c
codebookd.$(OBJEXT): codebookd.c
codebookdvq.$(OBJEXT): codebookdvq.c

codebook.lo: codebook.c

codebook.c: generate_codebook $(CODEBOOKS)
	./generate_codebook lsp_cb $(CODEBOOKS) > codebook.c

codebookd.c: generate_codebook $(CODEBOOKSD)
	./generate_codebook  lsp_cbd $(CODEBOOKSD) > codebookd.c

codebookdvq.c: generate_codebook $(CODEBOOKSDVQ)
	./generate_codebook  lsp_cbdvq $(CODEBOOKSDVQ) > codebookdvq.c

clean-local:
	-rm -f codebook.c codebookd.c codebookdvq.c

lib_LTLIBRARIES	= libcodec2.la
libcodec2_la_SOURCES = dump.c \
lpc.c \
nlp.c \
postfilter.c \
sine.c \
codec2.c \
fft.c \
kiss_fft.c \
interp.c \
lsp.c \
phase.c \
quantise.c \
pack.c \
codebook.c \
codebookd.c \
codebookdvq.c

libcodec2_la_CFLAGS = $(AM_CFLAGS)
libcodec2_la_LDFLAGS = $(LIBS)

generate_codebook_LDFLAGS = $(LIBS) -lm

library_includedir = $(prefix)
library_include_HEADERS	= codec2.h \
defines.h \
kiss_fft.h\
_kiss_fft_guts.h \
fft.h \
interp.h \
lsp.h \
phase.h \
quantise.h \
comp.h \
dump.h \
lpc.h \
nlp.h  \
postfilter.h \
sine.h
