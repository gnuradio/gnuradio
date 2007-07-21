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
from gnuradio.gruimpl.freqz import freqz


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
    taps = gr.firdes.low_pass (1.0,           # gain
                               sample_rate,   # sampling rate
                               200e3,         # low pass cutoff freq
                               100e3,         # width of trans. band
                               gr.firdes.WIN_HAMMING)
    # print len (taps)
    return gnuplot_freqz (freqz (taps, 1), sample_rate)

if __name__ == '__main__':
    handle = test_plot ()
    raw_input ('Press Enter to continue: ')
