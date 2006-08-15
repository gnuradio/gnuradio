#
# Copyright 2001,2003,2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

# Install this stuff in the gr subdirectory of the python pkg dir.
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/gr

grgrpythondir = $(grpythondir)/gr
grgrlibdir    = $(grpyexecdir)/gr


INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) -I$(srcdir)


EXTRA_DIST = gen-swig-bug-fix


LOCAL_IFILES =				\
	gnuradio.i			\
	shared_ptr.i					

ALL_IFILES =				\
	$(LOCAL_IFILES)			


BUILT_SOURCES =				\
	gnuradio_swig_python.cc		\
	gnuradio_swig_python.py		\
	gnuradio_swig_python.h		\
	gnuradio_swig_bug_workaround.h	

grgrpython_PYTHON = 			\
	gnuradio_swig_python.py		


SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(INCLUDES)


# ----------------------------------------------------------------
# _gnuradio_swig_python contains all the glue that implements
# the gnuradio.gr python package

grgrlib_LTLIBRARIES = 					\
	_gnuradio_swig_python.la

_gnuradio_swig_python_la_SOURCES = 			\
	gnuradio_swig_python.cc		


_gnuradio_swig_python_la_LIBADD  = 			\
	$(top_builddir)/gnuradio-core/src/lib/libgnuradio-core.la	\
	$(PYTHON_LDFLAGS)				\
	-lstdc++

_gnuradio_swig_python_la_LDFLAGS = -module -avoid-version $(NO_UNDEFINED)


# KLUDGE: Force runtime include of gnuradio_swig_python.d dependency file.
# This is not guaranteed to be portable, but will probably work.
# If it works, we have accurate dependencies for our swig stuff, which is good.
@am__include@ @am__quote@./gnuradio_swig_python.d@am__quote@

gnuradio_swig_python.py gnuradio_swig_python.h: gnuradio_swig_python.cc

gnuradio_swig_python.cc : gnuradio.i
	if $(SWIG) $(SWIGPYTHONARGS) -MMD -MF gnuradio_swig_python.Td -module gnuradio_swig_python -o gnuradio_swig_python.cc gnuradio.i ;\
	then if test $(host_os) = mingw32; \
	     then sed 's,\\\\,/,g' <gnuradio_swig_python.Td >gnuradio_swig_python.d; rm -f gnuradio_swig_python.Td; \
	     else mv -f gnuradio_swig_python.Td gnuradio_swig_python.d; fi \
	else rm -f gnuradio_swig_python.Td; exit 1; fi 


gnuradio_swig_bug_workaround.h : gnuradio_swig_python.cc $(srcdir)/gen-swig-bug-fix
	$(srcdir)/gen-swig-bug-fix gnuradio_swig_python.cc $@


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

DISTCLEANFILES = gnuradio_swig_python.d
