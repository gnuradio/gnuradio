#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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
from usrp_source import usrp_source_c
from gmsk2 import gmsk2_demod

_dect_symbol_rate = 1.152e6
_dect_occupied_bandwidth = _dect_symbol_rate * 1.03 # BT=0.5
_dect_channel_bandwidth = 1.728e6

# Top-level hierarchical block that implements DECT demodulation and
# decoding.
class dect_receiver(gr.top_block):
    def __init__(self, options):
	gr.top_block.__init__(self, "dect_receiver")
        self._options = options

        # Need greater than 2 samples per symbol. This makes a decimation
        # rate of 26 and a samples per symbol of 2.136752
        if_rate = 2.461538e6
        self._usrp = usrp_source_c(which=0,
                                   subdev_spec=options.rx_subdev_spec,
                                   if_rate=if_rate,
                                   gain=options.gain,
                                   freq=options.freq,
                                   calibration=options.calibration,
                                   verbose=options.verbose)

        # Filter baseband to 1.186 MHz passband, 1.728 MHz stopband
        chan_taps = optfir.low_pass(1.0,              # Gain
                                    if_rate,          # Sample rate
                                    _dect_occupied_bandwidth/2, # One sided modulation bandwidth
                                    _dect_channel_bandwidth/2,  # One sided channel bandwidth
                                    1.0,              # Passband ripple (db)
                                    60)               # Stopband attenuation (db)
        if self._options.verbose:
            print "Channel filter has", len(chan_taps), "taps"
        self._channel_filter = gr.freq_xlating_fir_filter_ccf(1,         # Decimation rate
                                                              chan_taps, # Filter taps
                                                              0.0,       # Offset frequency
                                                              if_rate)   # Sample rate

        self._demod = gmsk2_demod(samples_per_symbol=if_rate/_dect_symbol_rate,
                                  verbose=options.verbose)

        self._sink = gr.null_sink(gr.sizeof_char)
        self.connect(self._usrp, self._channel_filter, self._demod, self._sink)

        # Log baseband to file if requested
        if options.log_baseband is not None:
            if options.verbose:
                print "Logging baseband to file", options.log_baseband
            self.baseband_log = gr.file_sink(gr.sizeof_gr_complex, options.log_baseband)
            self.connect(self._channel_filter, self.baseband_log)

        # Log demodulator output to file if requested
        if options.log_demod is not None:
            if options.verbose:
                print "Logging demodulator to file", options.log_demod
            self.demod_log = gr.file_sink(gr.sizeof_char, options.log_demod)
            self.connect(self._demod, self.demod_log)
