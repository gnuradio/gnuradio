#!/usr/bin/env python
#
# Copyright 2004,2007,2012 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import audio
from gnuradio import analog

def build_graph():
    sampling_freq = 32000
    ampl = 0.1

    tb = gr.top_block()
    src0 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE, 350, ampl)
    src1 = analog.sig_source_f(sampling_freq, analog.GR_SIN_WAVE, 440, ampl)
    dst = audio.sink(sampling_freq)
    tb.connect(src0, (dst, 0))
    tb.connect(src1, (dst, 1))

    return tb

if __name__ == '__main__':
    tb = build_graph()
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
