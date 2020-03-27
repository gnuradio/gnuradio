#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Fourier-transform blocks and related functions.
'''

from __future__ import absolute_import
from __future__ import unicode_literals

import os

try:
    from .fft_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .fft_swig import *

# Shim the appropriate python fft library here
fft_detail = None
try:
    from numpy.fft import fftpack as fft_detail
except ImportError:
    # Numpy changed fft implementation in version 1.17
    # from fftpack to pocketfft
    try:
        from numpy.fft import pocketfft as fft_detail
    except ImportError:
        try:
            from numpy.fft import fft as fft_detail
        except ImportError:
            try:
                from scipy import fft as fft_detail
            except ImportError:
                raise ImportError("Unable to load FFT library from NumPy or SciPy")
        