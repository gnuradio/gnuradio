#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

#
# All Your Frequencies are Belong to Us!
#
#   Transmit NBFM message on 25 channels simultaneously!
#

from gnuradio import gr, gru, eng_notation
from gnuradio import usrp
from gnuradio import audio
from gnuradio import blks
from gnuradio import optfir
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import math
import sys
import random

from gnuradio.wxgui import stdgui, fftsink
import wx


def make_random_complex_tuple(L):
    result = []
    for x in range(L):
        result.append(complex(random.gauss(0, 1),random.gauss(0, 1)))
                      
    return tuple(result)

def random_noise_c():
    src = gr.vector_source_c(make_random_complex_tuple(32*1024), True)
    return src


def plot_taps(taps, sample_rate=2):
    return gru.gnuplot_freqz (gru.freqz (taps, 1), sample_rate)
    

class ayfabtu_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        parser = OptionParser (option_class=eng_option)
        parser.add_option ("-c", "--duc-freq", type="eng_float", default=29.325e6,
                           help="set Tx ddc frequency to FREQ", metavar="FREQ")
        (options, args) = parser.parse_args ()

        nchan = 25
        IF_GAIN = 80000
        AUDIO_GAIN = 100
        
        self.dac_rate = 128e6
        self.usrp_interp = 256
        self.usrp_rate = self.dac_rate / self.usrp_interp    # 500 kS/s
        self.audio_rate = 32000                              # 32 kS/s

        self.audio_src = gr.file_source(gr.sizeof_float, "ayfabtu.dat", True)

        ahp_taps = gr.firdes.high_pass(1,     # gain
                                       32e3,  # Fs
                                       300,   # cutoff
                                       600,   # trans width
                                       gr.firdes.WIN_HANN)
        self.audio_hp = gr.fir_filter_fff(1, ahp_taps)

        self.audio_gain = gr.multiply_const_ff(AUDIO_GAIN)

        null_src = gr.null_source(gr.sizeof_gr_complex)
        #noise_src = gr.noise_source_c(gr.GR_UNIFORM, 1, 0)
        noise_src = random_noise_c()

        if 0:
            artaps = optfir.low_pass(1,       # gain
                                     2,       # Fs
                                     .75/32,  # freq1
                                     1.0/32,  # freq2
                                     1,       # pb ripple in dB
                                     50,      # stopband atten in dB
                                     2)       # + extra taps
        else:
            artaps = gr.firdes.low_pass(1,      # gain
                                        32e3*15,# Fs
                                        2.7e3,  # cutoff
                                         .3e3,  # trans width
                                        gr.firdes.WIN_HANN)
        print "len(artaps) =", len(artaps)
        self.audio_resampler = blks.rational_resampler_fff(self, 15, 32, artaps)

        self.fm_mod = blks.nbfm_tx(self, 15000, 15000, max_dev=4.5e3)


        fbtaps = gr.firdes.low_pass(1,                # gain
                                    25*15e3,          # rate
                                    13e3,             # cutoff
                                    2e3,              # trans width
                                    gr.firdes.WIN_HANN)
        print "len(fbtabs) =", len(fbtaps)
        #self.plot = plot_taps(fbtaps, 25*15e3)
        self.filter_bank = blks.synthesis_filterbank(self, nchan, fbtaps)
        
        self.if_gain = gr.multiply_const_cc(IF_GAIN)

        if 0:
            ifrtaps = optfir.low_pass(1,
                                      2,       # Fs
                                      .75/3,   # freq1
                                      1.0/3,   # freq2
                                      1,       # pb ripple in dB
                                      50,      # stopband atten in dB
                                      2)       # + extra taps
        else:
            ifrtaps = gr.firdes.low_pass(1,
                                         2,       # Fs
                                         .75/3,   # freq1
                                         .25/3,   # trans width
                                         gr.firdes.WIN_HANN)


        print "len(ifrtaps) =", len(ifrtaps)
        self.if_resampler = blks.rational_resampler_ccf(self, 4, 3, ifrtaps)


        self.u = usrp.sink_c(0, 256)
        self.u.set_tx_freq(0, options.duc_freq)
        self.u.set_pga(0, self.u.pga_max())

        # wire it all together
        
        self.connect(self.audio_src, self.audio_hp, self.audio_gain,
                     self.audio_resampler, self.fm_mod)

        null_sink = gr.null_sink(gr.sizeof_gr_complex)

        for i in range(nchan):
            if True or i == 0:
                self.connect(self.fm_mod, (self.filter_bank, i))
            else:
                self.connect(null_src, (self.filter_bank, i))

        self.connect(self.filter_bank, self.if_gain, self.if_resampler, self.u)
        

def main ():
    app = stdgui.stdapp (ayfabtu_graph, "All Your Frequency Are Belong to Us")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
