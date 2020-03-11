#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Filter blocks and related functions.
'''
from __future__ import absolute_import
from __future__ import unicode_literals

import os
from gnuradio.fft import window

try:
    from .filter_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .filter_swig import *

from .filterbank import *
from .freq_xlating_fft_filter import *
from .rational_resampler import *
from . import pfb
from . import optfir

# Pull this into the filter module
from .file_taps_loader import file_taps_loader
