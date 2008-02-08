#
# Copyright 2007,2008 Free Software Foundation, Inc.
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

# Installation locations
ourpythondir = $(grpythondir)/gpio
ourlibdir = $(grpyexecdir)/gpio

# List of Python files that will get installed into site-packages
ourpython_PYTHON = 			\
    __init__.py 			\
    gpio.py

# List of python files that will be installed onto $prefix/bin
bin_SCRIPTS = 				\
    gpio_rx_sfile.py			\
    gpio_usrp_siggen.py 		\
    gpio_usrp_fft.py

# List of python files that will get distributed in tarball
# but not installed anywhere on system
noinst_PYTHON = 			\
    qa_gpio.py

# Programs that get run by 'make check'
TESTS = run_tests

# Files to go into tarball not otherwise mentioned (except bin_SCRIPTS!)
EXTRA_DIST = run_tests.in $(bin_SCRIPTS)

MOSTLYCLEANFILES = *.pyo *.pyc *~
