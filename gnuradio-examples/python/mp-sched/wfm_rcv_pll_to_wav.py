#!/usr/bin/env python
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, eng_notation, optfir
from gnuradio import audio
from gnuradio import blks2
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math

class wfm_rx_block (gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        usage = "usage: %prog [options] input-samples-320kS.dat output.wav"
        parser=OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-V", "--volume", type="eng_float", default=None,
                          help="set volume (default is midpoint)")

        (options, args) = parser.parse_args()
        if len(args) != 2:
            parser.print_help()
            sys.exit(1)
        
        input_filename = args[0]
        output_filename = args[1]

        self.vol = 0
        
        # build graph
        
        self.src = gr.file_source(gr.sizeof_gr_complex, input_filename, False)

        adc_rate = 64e6                             # 64 MS/s
        usrp_decim = 200
        usrp_rate = adc_rate / usrp_decim           # 320 kS/s
        chanfilt_decim = 1
        demod_rate = usrp_rate / chanfilt_decim
        audio_decimation = 10
        audio_rate = demod_rate / audio_decimation  # 32 kHz


        chan_filt_coeffs = optfir.low_pass (1,           # gain
                                            usrp_rate,   # sampling rate
                                            80e3,        # passband cutoff
                                            115e3,       # stopband cutoff
                                            0.1,         # passband ripple
                                            60)          # stopband attenuation
        #print len(chan_filt_coeffs)
        chan_filt = gr.fir_filter_ccf (chanfilt_decim, chan_filt_coeffs)


        #self.guts = blks2.wfm_rcv (demod_rate, audio_decimation)
        self.guts = blks2.wfm_rcv_pll (demod_rate, audio_decimation)

        # FIXME rework {add,multiply}_const_* to handle multiple streams
        self.volume_control_l = gr.multiply_const_ff(self.vol)
        self.volume_control_r = gr.multiply_const_ff(self.vol)

        # wave file as final sink
        if 1:
            sink = gr.wavfile_sink(output_filename, 2, int(audio_rate), 16)
        else:
            sink = audio.sink (int (audio_rate),
                               options.audio_output,
                               False)   # ok_to_block
        
        # now wire it all together
        self.connect (self.src, chan_filt, self.guts)
        self.connect ((self.guts, 0), self.volume_control_l, (sink, 0))
        self.connect ((self.guts, 1), self.volume_control_r, (sink, 1))
        try:
          self.guts.stereo_carrier_pll_recovery.squelch_enable(True)
        except:
          pass
          #print "FYI: This implementation of the stereo_carrier_pll_recovery has no squelch implementation yet"

        if options.volume is None:
            g = self.volume_range()
            options.volume = float(g[0]+g[1])/2

        # set initial values

        self.set_vol(options.volume)
        try:
          self.guts.stereo_carrier_pll_recovery.set_lock_threshold(options.squelch)
        except:
          pass
          #print "FYI: This implementation of the stereo_carrier_pll_recovery has no squelch implementation yet"


    def set_vol (self, vol):
        g = self.volume_range()
        self.vol = max(g[0], min(g[1], vol))
        self.volume_control_l.set_k(10**(self.vol/10))
        self.volume_control_r.set_k(10**(self.vol/10))

    def volume_range(self):
        return (-20.0, 0.0, 0.5)
        

if __name__ == '__main__':
    tb = wfm_rx_block()
    try:
        tb.run()
    except KeyboardInterrupt:
        pass
