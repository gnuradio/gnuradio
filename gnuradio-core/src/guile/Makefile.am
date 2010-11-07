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

TESTS = run_guile_tests

EXTRA_DIST = \
	run_guile_tests.in

# These are the hand-code guile files for gnuradio-core.
#
# Swig/common.scm is glue that's required for the goops wrappers.
# gnuradio/export-safely.scm works around some problems in the goops generated wrappers.
# gnuradio/core.scm glues the 5 pieces of gnuradio_core_* into a single module.
# gnuradio/runtime-shim implements "guile friendly" versions of connect & disconnect.

nobase_guile_DATA = \
	Swig/common.scm \
	gnuradio/core.scm \
	gnuradio/export-safely.scm \
	gnuradio/runtime-shim.scm \
	srfi/srfi-64.scm

# noinst_DATA = \
# 	qa_0000_basics.scm \
# 	qa_0010_ctor_filter.scm \
# 	qa_0010_ctor_general.scm \
# 	qa_0010_ctor_gengen.scm \
# 	qa_0010_ctor_hier.scm \
# 	qa_0010_ctor_io.scm \
# 	qa_0010_ctor_runtime.scm
# 

CLEANFILES = qa_*.log
