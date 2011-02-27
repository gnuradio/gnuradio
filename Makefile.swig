# -*- Makefile -*-
#
# Copyright 2009,2010 Free Software Foundation, Inc.
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

## This makefile should be included using 
##     include $(top_srcdir)/Makefile.swig
## in Makefile.am's which require SWIG wrapping / compilation.
## For just installing .i files, this Makefile is not required.


## include the built Makefile.swig.gen, always the one from the srcdir
include $(srcdir)/Makefile.swig.gen


## swig flags
## -w511 turns off keyword argument warning
## "-outdir $(builddir)" writes all generated output files to
##   the local builddir (which should always be '.')
##   In some older autotools, $(builddir) is not defined, so
##   just use '.' instead.

SWIG_PYTHON_FLAGS =	\
	-fvirtual	\
	-python		\
	-modern		\
	-keyword	\
	-w511		\
	-outdir .

STD_SWIG_PYTHON_ARGS =				\
	$(SWIG_PYTHON_FLAGS)			\
	$(STD_DEFINES_AND_INCLUDES)		\
	$(WITH_SWIG_INCLUDES)			\
	$(WITH_INCLUDES)

# NOTE: -Linkage passive and -Linkage module don't define SWIG_init()
SWIG_GUILE_FLAGS =	\
	-guile		\
	-scm		\
	-scmstub	\
	-package gnuradio \
	-Linkage module \
	-proxy \
	-goopsprefix gr: \
	-emit-slot-accessors \
	-emit-setters \
	-outdir .

## standard swig flags used by most components

STD_SWIG_GUILE_ARGS =				\
	$(SWIG_GUILE_FLAGS)			\
	$(STD_DEFINES_AND_INCLUDES)		\
	$(WITH_SWIG_INCLUDES)			\
	$(WITH_INCLUDES)

## standard SWIG LD flags for library creation

STD_SWIG_LA_LD_FLAGS =		\
	$(PYTHON_LDFLAGS)	\
	-module			\
	-avoid-version		\
	$(NO_UNDEFINED)

## standard SWIG library additions for library creation

STD_SWIG_LA_LIB_ADD = -lstdc++

## standard SWIG CXXFLAGS
## This allows for code to be compiled with "-O1" instead of "-g -O2"
## for some systems, avoiding some optimization issues.

STD_SWIG_CXX_FLAGS = @swig_CXXFLAGS@

# We drive the dependencies off of swig_built_sources.  This variable
# ends up containing only the generated .py and/or .scm files, not the .h
# or .cc files.  This allows us to use the pattern rules defined
# below to generate all the pieces without the parallel make
# problems that occur when both the .py's and .cc's are in swig_built_sources.

swig_built_sources =

# swig_all_built_sources contains swig_built_sources plus the .cc and .h files.
# It contains the files to remove from the distribution and the files to
# remove for make clean.

swig_all_built_sources =

if PYTHON
# Create a list of .py files based on the top level .i files.
PYTHON_GEN = $(foreach IFILE,$(TOP_SWIG_IFILES), $(subst .i,.py,$(IFILE)))
swig_built_sources += $(PYTHON_GEN)
swig_all_built_sources += $(PYTHON_GEN)

# Now add .h, .cc to _all_
swig_all_built_sources += $(foreach IFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,python/%.h,$(IFILE)))
swig_all_built_sources += $(foreach IFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,python/%.cc,$(IFILE)))
endif

if GUILE
# Create a list of .scm files based on the top level .i files.
GUILE_GEN = $(foreach IFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,gnuradio/%.scm,$(IFILE)))
swig_built_sources += $(GUILE_GEN)
swig_all_built_sources += $(GUILE_GEN)

# Now add -primitive.scm, .cc to _all_
swig_all_built_sources += $(foreach IFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,gnuradio/%-primitive.scm,$(IFILE)))
swig_all_built_sources += $(foreach IFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,guile/%.cc,$(IFILE)))
endif

# N.B. Only $(swig_built_sources), not $(swig_all_built_sources)
BUILT_SOURCES += $(swig_built_sources)

# Don't distribute any of the swig generated files
no_dist_files = $(swig_all_built_sources)

CLEANFILES += $(swig_all_built_sources)
CLEANFILES += python/*.lo python/*.o python/*.d
CLEANFILES += guile/*.lo  guile/*.o  guile/*.d


## SWIG suffixes for automake to know about
SUFFIXES = .i .scm .py

# Compile a .i to what guile needs. We use -o to set the output file name,
# or even with -outdir guile in SWIG_GUILE_ARGS, swig keeps putting a 
# gnuradio_core_*_wrap.cxx in the source directory.
gnuradio/%.scm : %.i
	@echo "Compile .i to .scm"
	@test -d "guile" || $(mkinstalldirs) "guile"
	@test -d "gnuradio" || $(mkinstalldirs) "gnuradio"
	$(SWIG) $(STD_SWIG_GUILE_ARGS) $($*_swig_args) \
		-MD -MF guile/$*.Std \
		-module $* -o guile/$*.cc $<
	$(SED) -e 's|guile/\(.*\)\.cc:|gnuradio/\1.scm:|' guile/$*.Std > guile/$*.d
	$(SED) -i -e 's/<--dummy-[0-9]\+-->/<top>/g' gnuradio/$*.scm
	$(SED) -i -e 's/^(export /(export-safely /' gnuradio/$*.scm
	$(RM) guile/$*.Std

# Compile a .i file to what python needs
.i.py:
	@echo "Compile .i to .py"
	@test -d "python" || $(mkinstalldirs) "python"
	$(SWIG) $(STD_SWIG_PYTHON_ARGS) $($*_swig_args) \
		-MD -MF python/$*.Std \
		-module $* -o python/$*.cc -oh python/$*.h $<
	$(SED) -e 's|python/\(.*\)\.cc:|\1.py:|' python/$*.Std > python/$*.d
	$(RM) python/$*.Std

## ------------------------------------------------------------------------
## Rule that (re)generates Makefile.swig.gen using TOP_SWIG_IFILES and
## Makefile.swig.gen.t
##

## Create $(srcdir)/Makefile.swig.gen, containing all of the rules
## for running SWIG to generate or re-generate outputs.  SWIG file
## names are to be defined in TOP_SWIG_IFILES, and must include the
## full path to the file and full filename including extension.  This
## Makefile addition will be made only if either it does not exist or
## if the top-level template has been modified.

generate-makefile-swig $(srcdir)/Makefile.swig.gen: $(top_srcdir)/Makefile.swig.gen.t
## recreate $(srcdir)/Makefile.swig.gen only if ...
	@do_recreate=0; \
	if test -f $(srcdir)/Makefile.swig.gen; then \
## the file exists and can be removed; or ...
		if $(RM) $(srcdir)/Makefile.swig.gen 2>/dev/null; then \
			if touch $(srcdir)/Makefile.swig.gen 2>/dev/null; then \
				do_recreate=1; \
			fi; \
		fi; \
	else \
## the file doesn't exist, but can be created (e.g., by touching it).
		if touch $(srcdir)/Makefile.swig.gen 2>/dev/null; then \
			do_recreate=1; \
		fi; \
	fi; \
	if test "$$do_recreate" == "1"; then \
		echo "Regenerating $(srcdir)/Makefile.swig.gen"; \
		for TFILE in $(TOP_SWIG_IFILES); do \
## retrieve just the filename, without path or extension
			TNAME=`python -c "import os.path as op; (dN, fN) = op.split ('$$TFILE'); (fbN, fE) = op.splitext (fN); print fbN;"`; \
## Replace the @-named strings in the template Makefile for SWIG.
			$(SED) -e 's|@NAME@|'$$TNAME'|g;' < $(top_srcdir)/Makefile.swig.gen.t >> $(srcdir)/Makefile.swig.gen; \
			echo "" >> $(srcdir)/Makefile.swig.gen; \
		done; \
	else \
		echo "Cannot recreate $(srcdir)/Makefile.swig.gen because the directory or file is write-protected."; \
		exit -1; \
	fi;

