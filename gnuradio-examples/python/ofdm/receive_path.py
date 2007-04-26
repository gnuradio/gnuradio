#!/usr/bin/env python
#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, blks
from gnuradio import usrp
from gnuradio import eng_notation
import copy
import sys

# from current dir
from pick_bitrate import pick_rx_bitrate
import ofdm

# /////////////////////////////////////////////////////////////////////////////
#                              receive path
# /////////////////////////////////////////////////////////////////////////////

class receive_path(gr.hier_block):
    def __init__(self, fg, rx_callback, options):

        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose     = options.verbose
        self._log         = options.log
        self._rx_callback = rx_callback      # this callback is fired when there's a packet available

        if 0:
            
            chan_coeffs = gr.firdes.low_pass (1.0,                  # gain
                                              2.0,                  # sampling rate
                                              bw,                   # midpoint of trans. band
                                              1*bw,               # width of trans. band
                                              gr.firdes.WIN_KAISER)   # filter type 
        else:
            chan_coeffs = [ 1.0, 0.0, 0 ] 
        self.chan_filt = gr.fft_filter_ccc(1, chan_coeffs)

        self.ofdm_demod = ofdm.ofdm_demod(fg, options)
        
        # receiver
        self.packet_receiver = \
            blks.demod_ofdm_pkts(fg,
                                 self.ofdm_demod,
                                 access_code=None,
                                 callback=self._rx_callback,
                                 threshold=-1)

        # Carrier Sensing Blocks
        alpha = 0.001
        thresh = 30   # in dB, will have to adjust
        self.probe = gr.probe_avg_mag_sqrd_c(thresh,alpha)
        fg.connect(self.chan_filt, self.probe)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()
        
        fg.connect(self.chan_filt, self.packet_receiver)
        #fg.connect(self.chan_filt, gr.file_sink(gr.sizeof_gr_complex, "ofdmrx_chflt.dat"))
        gr.hier_block.__init__(self, fg, self.chan_filt, None)

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
        normal.add_option("-v", "--verbose", action="store_true", default=False)
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to files (CAUTION: lots of data)")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)


    def _print_verbage(self):
        """
        Prints information about the receive path
        """
        print "Using RX d'board %s"    % (self.subdev.side_and_name(),)
        print "Rx gain:         %g"    % (self.gain,)
        print "modulation:      %s"    % (self._demod_class.__name__)
        print "bitrate:         %sb/s" % (eng_notation.num_to_str(self._bitrate))
        print "samples/symbol:  %3d"   % (self._samples_per_symbol)
        print "decim:           %3d"   % (self._decim)
