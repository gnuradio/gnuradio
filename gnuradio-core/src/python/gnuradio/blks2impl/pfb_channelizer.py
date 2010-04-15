#!/usr/bin/env python
#
# Copyright 2009,2010 Free Software Foundation, Inc.
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

class pfb_channelizer_ccf(gr.hier_block2):
    '''
    Make a Polyphase Filter channelizer (complex in, complex out, floating-point taps)

    This simplifies the interface by allowing a single input stream to connect to this block.
    It will then output a stream for each channel.
    '''
    def __init__(self, numchans, taps, oversample_rate=1):
	gr.hier_block2.__init__(self, "pfb_channelizer_ccf",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(numchans, numchans, gr.sizeof_gr_complex)) # Output signature

        self._numchans = numchans
        self._taps = taps
        self._oversample_rate = oversample_rate

        self.s2ss = gr.stream_to_streams(gr.sizeof_gr_complex, self._numchans)
        self.pfb = gr.pfb_channelizer_ccf(self._numchans, self._taps,
                                          self._oversample_rate)
        self.v2s = gr.vector_to_streams(gr.sizeof_gr_complex, self._numchans)

        self.connect(self, self.s2ss)

        for i in xrange(self._numchans):
            self.connect((self.s2ss,i), (self.pfb,i))

        # Get independent streams from the filterbank and send them out
        self.connect(self.pfb, self.v2s)

        for i in xrange(self._numchans):
            self.connect((self.v2s,i), (self,i))

        
        
        
