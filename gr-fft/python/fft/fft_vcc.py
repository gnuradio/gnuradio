#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from . import fft_python as fft


class fft_vcc():
    def __new__(self, fft_size,
                forward,
                window,
                shift=False,
                nthreads=1):
        if forward:
            return fft.fft_vcc_fwd(fft_size, window, shift, nthreads)
        else:
            return fft.fft_vcc_rev(fft_size, window, shift, nthreads)
