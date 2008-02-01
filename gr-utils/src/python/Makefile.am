#
# Copyright 2007 Free Software Foundation, Inc.
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

EXTRA_DIST = \
    $(bin_SCRIPTS) \
    README.plot

ourpythondir = $(grpythondir)

ourpython_PYTHON = \
    plot_data.py

bin_SCRIPTS = \
    gr_plot_char.py \
    gr_plot_const.py \
    gr_plot_fft_c.py \
    gr_plot_fft_f.py \
    gr_plot_float.py \
    gr_plot_int.py \
    gr_plot_iq.py \
    gr_plot_short.py \
    usrp_fft.py \
    usrp_oscope.py \
    usrp_print_db.py \
    usrp_rx_cfile.py \
    usrp_rx_nogui.py \
    usrp_siggen.py \
    usrp_test_counting.py \
    usrp_test_loopback.py

MOSTLYCLEANFILES = *~ *.pyc
