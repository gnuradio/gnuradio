#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# The presence of this file turns this directory into a Python package

'''
Blocks and utilities for digital TV module.
'''
from __future__ import absolute_import
from __future__ import unicode_literals

import os

try:
    from .dtv_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .dtv_swig import *

# Import pure python code here
from .atsc_rx import *
