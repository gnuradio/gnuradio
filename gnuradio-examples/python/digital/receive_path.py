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

from gnuradio import gr, gru, blks2
from gnuradio import eng_notation
import copy
import sys

# /////////////////////////////////////////////////////////////////////////////
#                              receive path
# /////////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block2):
    def __init__(self, demod_class, rx_callback, options):
	gr.hier_block2.__init__(self, "receive_path",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(0, 0, 0))                    # Output signature

        
        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose            = options.verbose
        self._bitrate            = options.bitrate         # desired bit rate
        self._samples_per_symbol = options.samples_per_symbol  # desired samples/symbol

        self._rx_callback   = rx_callback      # this callback is fired when there's a packet available
        self._demod_class   = demod_class      # the demodulator_class we're using

        # Get demod_kwargs
        demod_kwargs = self._demod_class.extract_kwargs_from_options(options)

        # Design filter to get actual channel we want
        sw_decim = 1
        chan_coeffs = gr.firdes.low_pass (1.0,                  # gain
                                          sw_decim * self._samples_per_symbol, # sampling rate
                                          1.0,                  # midpoint of trans. band
                                          0.5,                  # width of trans. band
                                          gr.firdes.WIN_HANN)   # filter type
        self.channel_filter = gr.fft_filter_ccc(sw_decim, chan_coeffs)
        
        # receiver
        self.packet_receiver = \
            blks2.demod_pkts(self._demod_class(**demod_kwargs),
                             access_code=None,
                             callback=self._rx_callback,
                             threshold=-1)

        # Carrier Sensing Blocks
        alpha = 0.001
        thresh = 30   # in dB, will have to adjust
        self.probe = gr.probe_avg_mag_sqrd_c(thresh,alpha)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

	# connect block input to channel filter
	self.connect(self, self.channel_filter)

        # connect the channel input filter to the carrier power detector
        self.connect(self.channel_filter, self.probe)

        # connect channel filter to the packet receiver
        self.connect(self.channel_filter, self.packet_receiver)

    def bitrate(self):
        return self._bitrate

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def carrier_sensed(self):
        """
        Return True if we think carrier is present.
        """
        #return self.probe.level() > X
        return self.probe.unmuted()

    def carrier_threshold(self):
        """
        Return current setting in dB.
        """
        return self.probe.threshold()

    def set_carrier_threshold(self, threshold_in_db):
        """
        Set carrier threshold.

        @param threshold_in_db: set detection threshold
        @type threshold_in_db:  float (dB)
        """
        self.probe.set_threshold(threshold_in_db)
    
        
    def add_options(normal, expert):
        """
        Adds receiver-specific options to the Options Parser
        """
        if not normal.has_option("--bitrate"):
            normal.add_option("-r", "--bitrate", type="eng_float", default=100e3,
                              help="specify bitrate [default=%default].")
        normal.add_option("", "--show-rx-gain-range", action="store_true", default=False, 
                          help="print min and max Rx gain available on selected daughterboard")
        normal.add_option("-v", "--verbose", action="store_true", default=False)
        expert.add_option("-S", "--samples-per-symbol", type="int", default=2,
                          help="set samples/symbol [default=%default]")
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to files (CAUTION: lots of data)")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)


    def _print_verbage(self):
        """
        Prints information about the receive path
        """
        print "\nReceive Path:"
        print "modulation:      %s"    % (self._demod_class.__name__)
        print "bitrate:         %sb/s" % (eng_notation.num_to_str(self._bitrate))
        print "samples/symbol:  %3d"   % (self._samples_per_symbol)
