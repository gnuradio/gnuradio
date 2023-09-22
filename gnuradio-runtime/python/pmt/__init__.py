#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
vector, dict, uniform_vector, any (std::any cast)
'''


import os

try:
    from .pmt_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .pmt_python import *

# due to changes in the PMT_NIL singleton for static builds, we force
# this into Python here.
PMT_NIL = get_PMT_NIL()
PMT_T = get_PMT_T()
PMT_F = get_PMT_F()
PMT_EOF = get_PMT_EOF()

from .pmt_to_python import pmt_to_python as to_python
from .pmt_to_python import python_to_pmt as to_pmt
