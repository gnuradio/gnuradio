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

from gnuradio import gr, optfir

class pfb_arb_resampler_ccf(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single complex stream in and outputs a single complex
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
	gr.hier_block2.__init__(self, "pfb_arb_resampler_ccf",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature
        
        self._rate = rate
        self._size = flt_size

        if taps is not None:
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            #self._taps = gr.firdes.low_pass_2(self._size, self._size, bw, tb, atten)
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(self._size, self._size, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = gr.pfb_arb_resampler_ccf(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)
        
        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)


class pfb_arb_resampler_fff(gr.hier_block2):
    '''
    Convenience wrapper for the polyphase filterbank arbitrary resampler.

    The block takes a single float stream in and outputs a single float
    stream out. As such, it requires no extra glue to handle the input/output
    streams. This block is provided to be consistent with the interface to the
    other PFB block.
    '''
    def __init__(self, rate, taps=None, flt_size=32, atten=100):
	gr.hier_block2.__init__(self, "pfb_arb_resampler_fff",
				gr.io_signature(1, 1, gr.sizeof_float), # Input signature
				gr.io_signature(1, 1, gr.sizeof_float)) # Output signature
        
        self._rate = rate
        self._size = flt_size

        if taps is not None:
            self._taps = taps
        else:
            # Create a filter that covers the full bandwidth of the input signal
            bw = 0.4
            tb = 0.2
            ripple = 0.1
            #self._taps = gr.firdes.low_pass_2(self._size, self._size, bw, tb, atten)
            made = False
            while not made:
                try:
                    self._taps = optfir.low_pass(self._size, self._size, bw, bw+tb, ripple, atten)
                    made = True
                except RuntimeError:
                    ripple += 0.01
                    made = False
                    print("Warning: set ripple to %.4f dB. If this is a problem, adjust the attenuation or create your own filter taps." % (ripple))

                    # Build in an exit strategy; if we've come this far, it ain't working.
                    if(ripple >= 1.0):
                        raise RuntimeError("optfir could not generate an appropriate filter.")

        self.pfb = gr.pfb_arb_resampler_fff(self._rate, self._taps, self._size)
        #print "PFB has %d taps\n" % (len(self._taps),)
        
        self.connect(self, self.pfb)
        self.connect(self.pfb, self)

    # Note -- set_taps not implemented in base class yet
    def set_taps(self, taps):
        self.pfb.set_taps(taps)

    def set_rate(self, rate):
        self.pfb.set_rate(rate)
