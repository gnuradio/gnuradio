#!/usr/bin/env python
#
# Copyright 2009 Free Software Foundation, Inc.
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

class pfb_decimator_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter decimator (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream that is the decimated output stream.
    '''
    def __init__(self, decim, taps, channel=0):
	gr.hier_block2.__init__(self, "pfb_decimator_ccf",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._decim = decim
        self._taps = taps
        self._channel = channel

        self.s2ss = gr.stream_to_streams(gr.sizeof_gr_complex, self._decim)
        self.pfb = gr.pfb_decimator_ccf(self._decim, self._taps, self._channel)

        self.connect(self, self.s2ss)

        for i in xrange(self._decim):
            self.connect((self.s2ss,i), (self.pfb,i))

        self.connect(self.pfb, self)
