#
# Copyright 2011 Free Software Foundation, Inc.
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

'''
Polymorphic Types.

The type can really be used to store anything, but also has simple
conversion methods for common data types such as bool, long, or a
vector.

The polymorphic type simplifies message passing between blocks, as all
of the data is of the same type, including the message. Tags also use
PMTs as data type, so a stream tag can be of any logical data type. In
a sense, PMTs are a way to extend C++' strict typing with something
more flexible.

The PMT library supports the following major types:
bool, symbol (string), integer, real, complex, null, pair, list,
vector, dict, uniform_vector, any (boost::any cast)
'''

import os

try:
    from pmt_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from pmt_swig import *

# due to changes in the PMT_NIL singleton for static builds, we force
# this into Python here.
PMT_NIL = get_PMT_NIL()
PMT_T = get_PMT_T()
PMT_F = get_PMT_F()
PMT_EOF = get_PMT_EOF()

from pmt_to_python import pmt_to_python as to_python
from pmt_to_python import python_to_pmt as to_pmt
