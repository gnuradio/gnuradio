#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, optfir
from math import pi
import pager_swig

class flex_demod:
    """
    FLEX pager protocol demodulation block.

    This block demodulates a band-limited, complex down-converted baseband 
    channel into FLEX protocol frames.

    Flow graph (so far):

    QUAD     - Quadrature demodulator converts FSK to baseband amplitudes  
    LPF      - Low pass filter to remove noise prior to slicer
    SLICER   - Converts input to one of four symbols (0, 1, 2, 3)
    SYNC     - Converts symbol stream to four phases of FLEX blocks
    DEINTx   - Deinterleaves FLEX blocks into datawords
    PARSEx   - Parse a single FLEX phase worth of data words into pages
    ---

    @param fg: flowgraph
    @param channel_rate:  incoming sample rate of the baseband channel
    @type sample_rate: integer
    """

    def __init__(self, fg, channel_rate):
        k = channel_rate/(2*pi*4800)        # 4800 Hz max deviation
        QUAD = gr.quadrature_demod_cf(k)
	self.INPUT = QUAD
		
        taps = optfir.low_pass(1.0, channel_rate, 3200, 6400, 0.1, 60)
        LPF = gr.fir_filter_fff(1, taps)
        SLICER = pager_swig.slicer_fb(.001, .00001) # Attack, decay
	SYNC = pager_swig.flex_sync(channel_rate)
        fg.connect(QUAD, LPF, SLICER, SYNC)

	DEINTA = pager_swig.flex_deinterleave()
	PARSEA = pager_swig.flex_parse()

	DEINTB = pager_swig.flex_deinterleave()
	PARSEB = pager_swig.flex_parse()

	DEINTC = pager_swig.flex_deinterleave()
	PARSEC = pager_swig.flex_parse()

	DEINTD = pager_swig.flex_deinterleave()
	PARSED = pager_swig.flex_parse()
	
	fg.connect((SYNC, 0), DEINTA, PARSEA)
	fg.connect((SYNC, 1), DEINTB, PARSEB)
	fg.connect((SYNC, 2), DEINTC, PARSEC)
	fg.connect((SYNC, 3), DEINTD, PARSED)
