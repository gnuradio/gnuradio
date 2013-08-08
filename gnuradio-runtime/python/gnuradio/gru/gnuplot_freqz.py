#!/usr/bin/env python
#
# Copyright 2005,2007 Free Software Foundation, Inc.
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

__all__ = ['gnuplot_freqz']

import tempfile
import os
import math
import numpy

from gnuradio import gr
from gnuradio.gru.freqz import freqz


def gnuplot_freqz (hw, Fs=None, logfreq=False):

    """hw is a tuple of the form (h, w) where h is sequence of complex
    freq responses, and w is a sequence of corresponding frequency
    points.  Plot the frequency response using gnuplot.  If Fs is
    provide, use it as the sampling frequency, else use 2*pi.

    Returns a handle to the gnuplot graph. When the handle is reclaimed
    the graph is torn down."""

    data_file = tempfile.NamedTemporaryFile ()
    cmd_file = os.popen ('gnuplot', 'w')

    h, w = hw
    ampl = 20 * numpy.log10 (numpy.absolute (h) + 1e-9)
    phase = map (lambda x: math.atan2 (x.imag, x.real), h)

    if Fs:
        w *= (Fs/(2*math.pi))

    for freq, a, ph in zip (w, ampl, phase):
        data_file.write ("%g\t%g\t%g\n" % (freq, a, ph))

    data_file.flush ()

    cmd_file.write ("set grid\n")
    if logfreq:
        cmd_file.write ("set logscale x\n")
    else:
        cmd_file.write ("unset logscale x\n")
    cmd_file.write ("plot '%s' using 1:2 with lines\n" % (data_file.name,))
    cmd_file.flush ()

    return (cmd_file, data_file)


def test_plot ():
    sample_rate = 2.0e6
    #taps = firdes.low_pass(1, sample_rate, 200000, 100000, firdes.WIN_HAMMING)
    taps = (0.0007329441141337156, 0.0007755281985737383, 0.0005323155201040208,
            -7.679847761841656e-19, -0.0007277769618667662, -0.001415981911122799,
            -0.0017135187517851591, -0.001282231998629868, 1.61239866282397e-18,
            0.0018589380197227001, 0.0035909228026866913, 0.004260237794369459,
            0.00310456077568233, -3.0331308923229716e-18, -0.004244099836796522,
            -0.007970594801008701, -0.009214458055794239, -0.006562007591128349,
            4.714311174044374e-18, 0.008654761128127575, 0.01605774275958538,
            0.01841980405151844, 0.013079923577606678, -6.2821650235090215e-18,
            -0.017465557903051376, -0.032989680767059326, -0.03894065320491791,
            -0.028868533670902252, 7.388111706347014e-18, 0.04517475143074989,
            0.09890196472406387, 0.14991308748722076, 0.18646684288978577,
            0.19974154233932495, 0.18646684288978577, 0.14991308748722076,
            0.09890196472406387, 0.04517475143074989, 7.388111706347014e-18,
            -0.028868533670902252, -0.03894065320491791, -0.032989680767059326,
            -0.017465557903051376, -6.2821650235090215e-18, 0.013079923577606678,
            0.01841980405151844, 0.01605774275958538, 0.008654761128127575,
            4.714311174044374e-18, -0.006562007591128349, -0.009214458055794239,
            -0.007970594801008701, -0.004244099836796522, -3.0331308923229716e-18,
            0.00310456077568233, 0.004260237794369459, 0.0035909228026866913,
            0.0018589380197227001, 1.61239866282397e-18, -0.001282231998629868,
            -0.0017135187517851591, -0.001415981911122799, -0.0007277769618667662,
            -7.679847761841656e-19, 0.0005323155201040208, 0.0007755281985737383,
            0.0007329441141337156)

    # print len (taps)
    return gnuplot_freqz (freqz (taps, 1), sample_rate)

if __name__ == '__main__':
    handle = test_plot ()
    raw_input ('Press Enter to continue: ')
