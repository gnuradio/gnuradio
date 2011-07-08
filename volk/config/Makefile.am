#
# Copyright 2001 Free Software Foundation, Inc.
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

# Install m4 macros in this directory
m4datadir = $(datadir)/aclocal

# List your m4 macros here
m4macros = \
	acx_pthread.m4 \
	ax_boost_base.m4 \
	ax_boost_unit_test_framework.m4 \
	bnv_have_qt.m4 \
	gr_lib64.m4 \
	gr_libgnuradio_core_extra_ldflags.m4 \
	gr_no_undefined.m4 \
	gr_omnithread.m4 \
	gr_pwin32.m4 \
	gr_set_md_cpu.m4 \
	lv_configure.m4 \
	gr_sysv_shm.m4 \
	lf_cc.m4 \
	lf_cxx.m4 \
	lf_warnings.m4 \
	lf_x11.m4 \
	$(top_gendir)/config/lv_set_simd_flags.m4 \
	mkstemp.m4 \
	onceonly.m4 \
	pkg.m4 \
	orc.m4 \
	gcc_version_workaround.m4




# Don't install m4 macros anymore
# m4data_DATA = $(m4macros) 

EXTRA_DIST = $(m4macros) 
