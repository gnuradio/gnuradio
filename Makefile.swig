# -*- Makefile -*-
#
# Copyright 2009 Free Software Foundation, Inc.
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

## standard swig flags used by most components

STD_SWIG_PYTHON_ARGS =				\
	$(SWIG_PYTHON_FLAGS)			\
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

STD_SWIG_LA_LIB_ADD =  		\
	-lstdc++

## standard SWIG CXXFLAGS
## This allows for code to be compiled with "-O1" instead of "-g -O2"
## for some systems, avoiding some optimization issues.

STD_SWIG_CXX_FLAGS = @swig_CXXFLAGS@

## SWIG suffix for automake to know about

SUFFIXES = .i

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

swig_built_sources =

## include the built Makefile.swig.gen, always the one from the
## srcdir; this must be included as the last item, because it depends
## on variables defined above.

include $(srcdir)/Makefile.swig.gen
