#
# Copyright 2010 Free Software Foundation, Inc.
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include $(top_srcdir)/Makefile.common

if GUILE

TESTS = run_guile_tests

EXTRA_DIST += \
	run_guile_tests.in \
	$(nobase_guile_DATA) \
	$(GUILE_TESTS)

bin_SCRIPTS = \
	gr-run-waveform-script

# These are the hand-coded guile files for gnuradio-core.
#
# Swig/common.scm is glue that's required for the goops wrappers.
# gnuradio/export-safely.scm works around some problems in the goops generated wrappers.
# gnuradio/core.scm glues the 5 pieces of gnuradio_core_* into a single module.
# gnuradio/runtime-shim implements "guile friendly" versions of connect & disconnect.

nobase_guile_DATA = \
	Swig/common.scm \
	gnuradio/core.scm \
	gnuradio/export-safely.scm \
	gnuradio/run-waveform.scm \
	gnuradio/runtime-shim.scm \
	gnuradio/waveform.scm \
	gnuradio/test-suite/guile-test \
	gnuradio/test-suite/lib.scm

GUILE_TESTS = \
	tests/00_runtime_basics.test \
	tests/00_runtime_ctors.test \
	tests/filter_ctors.test \
	tests/general_ctors.test \
	tests/gengen_ctors.test \
	tests/hier_ctors.test \
	tests/io_ctors.test

CLEANFILES = guile.log


scmlibdir = $(libdir)
scmdir = $(guiledir)

scmlib_LTLIBRARIES = libguile-gnuradio-dynl-global.la

libguile_gnuradio_dynl_global_la_SOURCES = dynl-global.c
libguile_gnuradio_dynl_global_la_CPPFLAGS = $(GUILE_CFLAGS)
libguile_gnuradio_dynl_global_la_LIBADD = $(GUILE_LIBS)

# Create a symlink from gr-run-waveform-script to gr-run-waveform
install-exec-local:
	-$(RM) $(DESTDIR)$(bindir)/gr-run-waveform
	(cd $(DESTDIR)$(bindir) && $(LN_S) gr-run-waveform-script gr-run-waveform)

uninstall-local:
	-$(RM) $(DESTDIR)$(bindir)/gr-run-waveform

endif
