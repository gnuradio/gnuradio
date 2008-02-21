#
# Copyright 2001,2003,2004,2005,2006,2007,2008 Free Software Foundation, Inc.
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

# Install this stuff in the gr subdirectory of the python pkg dir.
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/gr

grgrpythondir = $(grpythondir)/gr
grgrlibdir    = $(grpyexecdir)/gr

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) -I$(srcdir) \
	 $(WITH_INCLUDES)

EXTRA_DIST = gen-swig-bug-fix

LOCAL_IFILES =				\
	gnuradio.i			\
	shared_ptr.i			\
	sw_general.i			\
	sw_gengen.i			\
	sw_filter.i			\
	sw_io.i

ALL_IFILES =				\
	$(LOCAL_IFILES)

# ----------------------------------------------------------------
# We've split the previously monstrous gnuradio_swig_python into
# 5 smaller pieces.  This reduces compile time coupling and
# creates smaller pieces for the compiler to digest.
# prior to this change, on X86_64, g++'s resident set size was
# 650MB!

BUILT_SOURCES =				\
	gnuradio_swig_py_runtime.cc	\
	gnuradio_swig_py_runtime.py	\
	gnuradio_swig_py_runtime.h	\
	gnuradio_swig_py_general.cc	\
	gnuradio_swig_py_general.py	\
	gnuradio_swig_py_general.h	\
	gnuradio_swig_py_gengen.cc	\
	gnuradio_swig_py_gengen.py	\
	gnuradio_swig_py_gengen.h	\
	gnuradio_swig_py_filter.cc	\
	gnuradio_swig_py_filter.py	\
	gnuradio_swig_py_filter.h	\
	gnuradio_swig_py_io.cc		\
	gnuradio_swig_py_io.py		\
	gnuradio_swig_py_io.h		\
	gnuradio_swig_bug_workaround.h	

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(AM_CPPFLAGS) $(WITH_SWIG_INCLUDES) \
	$(WITH_INCLUDES)

# ----------------------------------------------------------------
# We break this up into 5 libraries...

grgrlib_LTLIBRARIES = 				\
	_gnuradio_swig_py_runtime.la		\
	_gnuradio_swig_py_general.la		\
	_gnuradio_swig_py_gengen.la		\
	_gnuradio_swig_py_filter.la		\
	_gnuradio_swig_py_io.la			


grgrpython_PYTHON = 			\
	gnuradio_swig_python.py		\
	gnuradio_swig_py_runtime.py	\
	gnuradio_swig_py_general.py	\
	gnuradio_swig_py_gengen.py	\
	gnuradio_swig_py_filter.py	\
	gnuradio_swig_py_io.py		


# This results in the code being compiled with -O1 instead of -g -O2

_gnuradio_swig_py_runtime_la_CXXFLAGS = @swig_CXXFLAGS@
_gnuradio_swig_py_general_la_CXXFLAGS = @swig_CXXFLAGS@
_gnuradio_swig_py_gengen_la_CXXFLAGS  = @swig_CXXFLAGS@
_gnuradio_swig_py_filter_la_CXXFLAGS  = @swig_CXXFLAGS@
_gnuradio_swig_py_io_la_CXXFLAGS      = @swig_CXXFLAGS@


# ----------------------------------------------------------------
# Part 1: gnuradio.i -> gnuradio_swig_py_runtime
#
# We continue to call this gnuradio.i to avoid having to touch
# lots of code in other modules

_gnuradio_swig_py_runtime_la_SOURCES = 		\
	gnuradio_swig_py_runtime.cc		


_gnuradio_swig_py_runtime_la_LIBADD  = 		\
	$(GNURADIO_CORE_LA)			\
	$(PYTHON_LDFLAGS)			\
	-lstdc++

_gnuradio_swig_py_runtime_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force runtime include of gnuradio_swig_py_runtime.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_py_runtime.d@am__quote@

gnuradio_swig_py_runtime.py gnuradio_swig_py_runtime.h: gnuradio_swig_py_runtime.cc

gnuradio_swig_py_runtime.cc : gnuradio.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_py_runtime.Td -module gnuradio_swig_py_runtime -o gnuradio_swig_py_runtime.cc gnuradio.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_py_runtime.Td >gnuradio_swig_py_runtime.d; rm -f gnuradio_swig_py_runtime.Td; \
	     else mv -f gnuradio_swig_py_runtime.Td gnuradio_swig_py_runtime.d; fi \
	else rm -f gnuradio_swig_py_runtime.Td; exit 1; fi 


# ----------------------------------------------------------------
# Part 2: sw_general.i -> gnuradio_swig_py_general
#

_gnuradio_swig_py_general_la_SOURCES = 		\
	gnuradio_swig_py_general.cc		


_gnuradio_swig_py_general_la_LIBADD  = 		\
	$(GNURADIO_CORE_LA)			\
	$(PYTHON_LDFLAGS)			\
	-lstdc++

_gnuradio_swig_py_general_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force general include of gnuradio_swig_py_general.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_py_general.d@am__quote@

gnuradio_swig_py_general.py gnuradio_swig_py_general.h: gnuradio_swig_py_general.cc

gnuradio_swig_py_general.cc : sw_general.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_py_general.Td -module gnuradio_swig_py_general -o gnuradio_swig_py_general.cc sw_general.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_py_general.Td >gnuradio_swig_py_general.d; rm -f gnuradio_swig_py_general.Td; \
	     else mv -f gnuradio_swig_py_general.Td gnuradio_swig_py_general.d; fi \
	else rm -f gnuradio_swig_py_general.Td; exit 1; fi 


# ----------------------------------------------------------------
# Part 3: sw_gengen.i -> gnuradio_swig_py_gengen
#

_gnuradio_swig_py_gengen_la_SOURCES = 		\
	gnuradio_swig_py_gengen.cc		


_gnuradio_swig_py_gengen_la_LIBADD  = 		\
	$(GNURADIO_CORE_LA)			\
	$(PYTHON_LDFLAGS)			\
	-lstdc++

_gnuradio_swig_py_gengen_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force gengen include of gnuradio_swig_py_gengen.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_py_gengen.d@am__quote@

gnuradio_swig_py_gengen.py gnuradio_swig_py_gengen.h: gnuradio_swig_py_gengen.cc

gnuradio_swig_py_gengen.cc : sw_gengen.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_py_gengen.Td -module gnuradio_swig_py_gengen -o gnuradio_swig_py_gengen.cc sw_gengen.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_py_gengen.Td >gnuradio_swig_py_gengen.d; rm -f gnuradio_swig_py_gengen.Td; \
	     else mv -f gnuradio_swig_py_gengen.Td gnuradio_swig_py_gengen.d; fi \
	else rm -f gnuradio_swig_py_gengen.Td; exit 1; fi 


# ----------------------------------------------------------------
# Part 4: sw_filter.i -> gnuradio_swig_py_filter
#

_gnuradio_swig_py_filter_la_SOURCES = 		\
	gnuradio_swig_py_filter.cc		


_gnuradio_swig_py_filter_la_LIBADD  = 		\
	$(GNURADIO_CORE_LA)			\
	$(PYTHON_LDFLAGS)			\
	-lstdc++

_gnuradio_swig_py_filter_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force filter include of gnuradio_swig_py_filter.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_py_filter.d@am__quote@

gnuradio_swig_py_filter.py gnuradio_swig_py_filter.h: gnuradio_swig_py_filter.cc

gnuradio_swig_py_filter.cc : sw_filter.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_py_filter.Td -module gnuradio_swig_py_filter -o gnuradio_swig_py_filter.cc sw_filter.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_py_filter.Td >gnuradio_swig_py_filter.d; rm -f gnuradio_swig_py_filter.Td; \
	     else mv -f gnuradio_swig_py_filter.Td gnuradio_swig_py_filter.d; fi \
	else rm -f gnuradio_swig_py_filter.Td; exit 1; fi 


# ----------------------------------------------------------------
# Part 5: sw_io.i -> gnuradio_swig_py_io
#

_gnuradio_swig_py_io_la_SOURCES = 		\
	gnuradio_swig_py_io.cc		


_gnuradio_swig_py_io_la_LIBADD  = 		\
	$(GNURADIO_CORE_LA)			\
	$(PYTHON_LDFLAGS)			\
	-lstdc++

_gnuradio_swig_py_io_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force io include of gnuradio_swig_py_io.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_py_io.d@am__quote@

gnuradio_swig_py_io.py gnuradio_swig_py_io.h: gnuradio_swig_py_io.cc

gnuradio_swig_py_io.cc : sw_io.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_py_io.Td -module gnuradio_swig_py_io -o gnuradio_swig_py_io.cc sw_io.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_py_io.Td >gnuradio_swig_py_io.d; rm -f gnuradio_swig_py_io.Td; \
	     else mv -f gnuradio_swig_py_io.Td gnuradio_swig_py_io.d; fi \
	else rm -f gnuradio_swig_py_io.Td; exit 1; fi 


# ----------------------------------------------------------------
# FIXME As of swig 1.3.31, this still seems to be required...

gnuradio_swig_bug_workaround.h : gnuradio_swig_py_runtime.cc $(srcdir)/gen-swig-bug-fix
	$(PYTHON) $(srcdir)/gen-swig-bug-fix gnuradio_swig_py_runtime.cc $@

# ----------------------------------------------------------------

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done


grinclude_HEADERS =				\
	gnuradio_swig_bug_workaround.h

swiginclude_HEADERS =				\
	$(LOCAL_IFILES)			

MOSTLYCLEANFILES = \
	$(BUILT_SOURCES) *~ *.pyc 

DISTCLEANFILES = 			\
	gnuradio_swig_py_runtime.d	\
	gnuradio_swig_py_general.d	\
	gnuradio_swig_py_gengen.d	\
	gnuradio_swig_py_filter.d	\
	gnuradio_swig_py_io.d		
