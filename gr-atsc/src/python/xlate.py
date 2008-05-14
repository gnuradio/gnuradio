#!/usr/bin/env /usr/bin/python
#
# Copyright 2004,2007 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 
# This module upconverts the 19.2e6 sample rate signal from a center
# of 0 to 5.75e6 and converts to float, to prepare the signal for
# the old gnuradio 0.9 block (bit timing loop, field sync checker,
# equalizer and field sync demux), effectively simulating an
# mc4020 card, except the sample rate is 19.2e6 instead of 20e6.
#
# The signal is then centered on 5.75e6 with edges at 5.75 + 3.2 = 8.95MHz
# and 5.75 - 3.2 = 2.55Mhz, low pass filtered with cutoff at 9Mhz and a
# transition band width of 1Mhz.
#
# Input complex -3.2 to 3.2Mhz, output float 2.55 to 8.95Mhz.

from gnuradio import gr
import os

def graph ():
    print os.getpid()
    sampling_freq = 19200000

    tb = gr.top_block ()

    src0 = gr.file_source (gr.sizeof_gr_complex,"/tmp/atsc_pipe_1")

    duc_coeffs = gr.firdes.low_pass ( 1, 19.2e6, 9e6, 1e6, gr.firdes.WIN_HAMMING )
    duc = gr.freq_xlating_fir_filter_ccf ( 1, duc_coeffs, 5.75e6, 19.2e6 )

    c2f = gr.complex_to_float()
    file = gr.file_sink(gr.sizeof_float,"/tmp/atsc_pipe_2")

    tb.connect( src0, duc, c2f, file )

    tb.run()

if __name__ == '__main__':
    graph ()
