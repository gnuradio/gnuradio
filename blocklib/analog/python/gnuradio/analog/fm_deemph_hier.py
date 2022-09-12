#
# Copyright 2005,2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, filter
import math


def fm_deemph_hier(self, fs, tau):
    # Digital corner frequency
    w_c = 1.0 / tau

    # Prewarped analog corner frequency
    w_ca = 2.0 * fs * math.tan(w_c / (2.0 * fs))

    # Resulting digital pole, zero, and gain term from the bilinear
    # transformation of H(s) = w_ca / (s + w_ca) to
    # H(z) = b0 (1 - z1 z^-1)/(1 - p1 z^-1)
    k = -w_ca / (2.0 * fs)
    z1 = -1.0
    p1 = (1.0 + k) / (1.0 - k)
    b0 = -k / (1.0 - k)

    btaps = [b0 * 1.0, b0 * -z1]
    ataps = [1.0, -p1]

    # Since H(s = 0) = 1.0, then H(z = 1) = 1.0 and has 0 dB gain at DC

    deemph = filter.iir_filter_ffd(btaps, ataps, False)
    self.connect((self, deemph, self))
