#
# Copyright 2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

AM_CFLAGS = -fPIC -O3

# Helper program to create codebook source
noinst_PROGRAMS = generate_codebook
generate_codebook_LDFLAGS = -lm

# lsp quantisers
CODEBOOKS= \
	$(srcdir)/codebook/lsp1.txt \
	$(srcdir)/codebook/lsp2.txt \
	$(srcdir)/codebook/lsp3.txt \
	$(srcdir)/codebook/lsp4.txt \
	$(srcdir)/codebook/lsp5.txt \
	$(srcdir)/codebook/lsp6.txt \
	$(srcdir)/codebook/lsp7.txt \
	$(srcdir)/codebook/lsp8.txt \
	$(srcdir)/codebook/lsp9.txt \
	$(srcdir)/codebook/lsp10.txt

# lspd quantisers
CODEBOOKSD= \
	$(srcdir)/codebook/dlsp1.txt \
	$(srcdir)/codebook/dlsp2.txt \
	$(srcdir)/codebook/dlsp3.txt \
	$(srcdir)/codebook/dlsp4.txt \
	$(srcdir)/codebook/dlsp5.txt \
	$(srcdir)/codebook/dlsp6.txt \
	$(srcdir)/codebook/dlsp7.txt \
	$(srcdir)/codebook/dlsp8.txt \
	$(srcdir)/codebook/dlsp9.txt \
	$(srcdir)/codebook/dlsp10.txt

# lspd VQ quantisers
CODEBOOKSDVQ= \
	$(srcdir)/codebook/dlsp1.txt \
	$(srcdir)/codebook/dlsp2.txt \
	$(srcdir)/codebook/dlsp3.txt \
	$(srcdir)/codebook/dlsp4.txt \
	$(srcdir)/codebook/dlsp5.txt 

# Generate codebook sources from text files
GENERATED_C = \
	codebook.c \
	codebookd.c \
	codebookdvq.c

BUILT_SOURCES += $(GENERATED_C)

codebook.c: $(builddir)/generate_codebook $(CODEBOOKS)
	$(builddir)/generate_codebook lsp_cb $(CODEBOOKS) > codebook.c

codebookd.c: $(builddir)/generate_codebook $(CODEBOOKSD)
	$(builddir)/generate_codebook lsp_cbd $(CODEBOOKSD) > codebookd.c

codebookdvq.c: $(builddir)/generate_codebook $(CODEBOOKSDVQ)
	$(builddir)/generate_codebook lsp_cbdvq $(CODEBOOKSDVQ) > codebookdvq.c

# Convenience library for linking into blocks
noinst_LTLIBRARIES = libcodec2.la

libcodec2_la_CFLAGS = $(AM_CFLAGS)

libcodec2_la_SOURCES = \
	dump.c \
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
	$(GENERATED_C)

# Evil inclusion of glottal.c by phase.c
EXTRA_DIST += glottal.c

# Headers used locally but not installed in system
noinst_HEADERS = \
	codec2.h \
	codec2_internal.h \
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

EXTRA_DIST += $(CODEBOOKS) $(CODEBOOKSD) $(CODEBOOKSDVQ)

