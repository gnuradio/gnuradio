#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Provides a GUI interface using the QT backend.
'''
from __future__ import unicode_literals

# The presence of this file turns this directory into a Python package
import os


try:
    from .qtgui_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .qtgui_swig import *

from .range import Range, RangeWidget
from . import util
