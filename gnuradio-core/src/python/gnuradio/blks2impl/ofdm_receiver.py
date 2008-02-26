#!/usr/bin/env python
#
# Copyright 2006, 2007 Free Software Foundation, Inc.
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

import math
from numpy import fft
from gnuradio import gr
from gnuradio.blks2impl.ofdm_sync_ml import ofdm_sync_ml
from gnuradio.blks2impl.ofdm_sync_pn import ofdm_sync_pn
from gnuradio.blks2impl.ofdm_sync_pnac import ofdm_sync_pnac
from gnuradio.blks2impl.ofdm_sync_fixed import ofdm_sync_fixed

class ofdm_receiver(gr.hier_block2):
    """
    Performs receiver synchronization on OFDM symbols.

    The receiver performs channel filtering as well as symbol, frequency, and phase synchronization.
    The synchronization routines are available in three flavors: preamble correlator (Schmidl and Cox),
    modifid preamble correlator with autocorrelation (not yet working), and cyclic prefix correlator
    (Van de Beeks).
    """

    def __init__(self, fft_length, cp_length, occupied_tones, snr, ks, logging=False):
        """
	Hierarchical block for receiving OFDM symbols.

	The input is the complex modulated signal at baseband.
        Synchronized packets are sent back to the demodulator.

        @param fft_length: total number of subcarriers
        @type  fft_length: int
        @param cp_length: length of cyclic prefix as specified in subcarriers (<= fft_length)
        @type  cp_length: int
        @param occupied_tones: number of subcarriers used for data
        @type  occupied_tones: int
        @param snr: estimated signal to noise ratio used to guide cyclic prefix synchronizer
        @type  snr: float
        @param ks: known symbols used as preambles to each packet
        @type  ks: list of lists
        @param logging: turn file logging on or off
        @type  logging: bool
	"""

	gr.hier_block2.__init__(self, "ofdm_receiver",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature2(2, 2, gr.sizeof_gr_complex*occupied_tones, gr.sizeof_char)) # Output signature

        bw = (float(occupied_tones) / float(fft_length)) / 2.0
        tb = bw*0.08
        chan_coeffs = gr.firdes.low_pass (1.0,                     # gain
                                          1.0,                     # sampling rate
                                          bw+tb,                   # midpoint of trans. band
                                          tb,                      # width of trans. band
                                          gr.firdes.WIN_HAMMING)   # filter type
        self.chan_filt = gr.fft_filter_ccc(1, chan_coeffs)
        
        win = [1 for i in range(fft_length)]

        zeros_on_left = int(math.ceil((fft_length - occupied_tones)/2.0))
        zeros_on_right = fft_length - occupied_tones - zeros_on_left
        ks0 = zeros_on_left*[0.0,]
        ks0.extend(ks[0])
        ks0.extend(zeros_on_right*[0.0,])
        
        ks0time = fft.ifft(ks0)
        # ADD SCALING FACTOR
        ks0time = ks0time.tolist()
        
        SYNC = "pn"
        if SYNC == "ml":
            self.ofdm_sync = ofdm_sync_ml(fft_length, cp_length, snr, logging)
        elif SYNC == "pn":
            self.ofdm_sync = ofdm_sync_pn(fft_length, cp_length, logging)
        elif SYNC == "pnac":
            self.ofdm_sync = ofdm_sync_pnac(fft_length, cp_length, ks0time)
        elif SYNC == "fixed":
            self.ofdm_sync = ofdm_sync_fixed(fft_length, cp_length, logging)
                        
        self.fft_demod = gr.fft_vcc(fft_length, True, win, True)
        self.ofdm_frame_acq = gr.ofdm_frame_acquisition(occupied_tones, fft_length,
                                                        cp_length, ks[0])

        self.connect(self, self.chan_filt)
        self.connect(self.chan_filt, self.ofdm_sync)
        self.connect((self.ofdm_sync,0), self.fft_demod, (self.ofdm_frame_acq,0))
        self.connect((self.ofdm_sync,1), (self.ofdm_frame_acq,1))
        self.connect((self.ofdm_frame_acq,0), (self,0))
        self.connect((self.ofdm_frame_acq,1), (self,1))

        if logging:
            self.connect(self.chan_filt, gr.file_sink(gr.sizeof_gr_complex, "chan_filt_c.dat"))
            self.connect(self.fft_demod, gr.file_sink(gr.sizeof_gr_complex*fft_length, "fft_out_c.dat"))
            self.connect(self.ofdm_frame_acq,
                         gr.file_sink(gr.sizeof_gr_complex*occupied_tones, "ofdm_frame_acq_c.dat"))
            self.connect((self.ofdm_frame_acq,1), gr.file_sink(1, "found_corr_b.dat"))
