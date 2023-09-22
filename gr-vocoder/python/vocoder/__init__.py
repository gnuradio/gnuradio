#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
This is the gr-vocoder package. This package includes the various
vocoder blocks in GNU Radio.
'''

import os

try:
    from .vocoder_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .vocoder_python import *

from .cvsd import *
