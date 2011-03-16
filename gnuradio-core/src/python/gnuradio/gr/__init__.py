#
# Copyright 2003,2004,2006,2008,2009,2010 Free Software Foundation, Inc.
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

# The presence of this file turns this directory into a Python package

# This is the main GNU Radio python module.
# We pull the swig output and the other modules into the gnuradio.gr namespace

# Temporary workaround for ticket:181.
# Use leading underscores to avoid namespace pollution
import sys
_RTLD_GLOBAL = 0
try:
    from dl import RTLD_GLOBAL as _RTLD_GLOBAL
except ImportError:
    try:
	from DLFCN import RTLD_GLOBAL as _RTLD_GLOBAL
    except ImportError:
	pass
    
if _RTLD_GLOBAL != 0:
    _dlopenflags = sys.getdlopenflags()
    sys.setdlopenflags(_dlopenflags|_RTLD_GLOBAL)

from gnuradio_core import *
from exceptions import *
from hier_block2 import *
from top_block import *

if _RTLD_GLOBAL != 0:
    sys.setdlopenflags(_dlopenflags)             # Restore original flags

# create a couple of aliases
serial_to_parallel = stream_to_vector
parallel_to_serial = vector_to_stream

# Force the preference database to be initialized
from prefs import prefs

#alias old gr_add_vXX and gr_multiply_vXX
add_vcc = add_cc
add_vff = add_ff
add_vii = add_ii
add_vss = add_ss
multiply_vcc = multiply_cc
multiply_vff = multiply_ff
multiply_vii = multiply_ii
multiply_vss = multiply_ss
