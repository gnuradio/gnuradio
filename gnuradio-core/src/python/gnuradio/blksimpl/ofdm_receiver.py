#!/usr/bin/env python
#
# Copyright 2006, 2007 Free Software Foundation, Inc.
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

import math
from gnuradio import gr
from gnuradio.blksimpl.ofdm_sync_ml import ofdm_sync_ml
from gnuradio.blksimpl.ofdm_sync_pn import ofdm_sync_pn
from gnuradio.blksimpl.ofdm_sync_pnac import ofdm_sync_pnac

class ofdm_receiver(gr.hier_block):
    def __init__(self, fg, fft_length, cp_length, occupied_tones, snr, ks, logging=False):
        self.fg = fg

        bw = (float(occupied_tones) / float(fft_length)) / 2.0
        tb = bw*0.08
        chan_coeffs = gr.firdes.low_pass (1.0,                     # gain
                                          1.0,                     # sampling rate
                                          bw+tb,                   # midpoint of trans. band
                                          tb,                      # width of trans. band
                                          gr.firdes.WIN_HAMMING)   # filter type
        self.chan_filt = gr.fft_filter_ccc(1, chan_coeffs)
        
        win = [1 for i in range(fft_length)]

        SYNC = "pn"
        if SYNC == "ml":
            self.ofdm_sync = ofdm_sync_ml(fg, fft_length, cp_length, snr, logging)
        elif SYNC == "pn":
            self.ofdm_sync = ofdm_sync_pn(fg, fft_length, cp_length, logging)
        elif SYNC == "pnac":
            self.ofdm_sync = ofdm_sync_pnac(fg, fft_length, cp_length, ks[0])

        self.fft_demod = gr.fft_vcc(fft_length, True, win, True)
        self.ofdm_corr  = gr.ofdm_correlator(occupied_tones, fft_length,
                                             cp_length, ks[1], ks[2])

        self.fg.connect(self.chan_filt, self.ofdm_sync, self.fft_demod, self.ofdm_corr)
        
        if logging:
            self.fg.connect(self.chan_filt, gr.file_sink(gr.sizeof_gr_complex, "chan_filt_c.dat"))
            self.fg.connect(self.fft_demod, gr.file_sink(gr.sizeof_gr_complex*fft_length, "fft_out_c.dat"))
            self.fg.connect(self.ofdm_corr, gr.file_sink(gr.sizeof_gr_complex*occupied_tones, "ofdm_corr_out_c.dat"))
            self.fg.connect((self.ofdm_corr,1), gr.file_sink(1, "found_corr_b.dat"))

        gr.hier_block.__init__(self, fg, self.chan_filt, self.ofdm_corr)
