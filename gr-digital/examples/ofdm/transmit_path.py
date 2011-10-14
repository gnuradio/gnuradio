#
# Copyright 2005,2006,2011 Free Software Foundation, Inc.
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
from gnuradio import eng_notation
from gnuradio import digital

import copy
import sys

# /////////////////////////////////////////////////////////////////////////////
#                              transmit path
# /////////////////////////////////////////////////////////////////////////////

class transmit_path(gr.hier_block2): 
    def __init__(self, options):
        '''
        See below for what options should hold
        '''

	gr.hier_block2.__init__(self, "transmit_path",
				gr.io_signature(0, 0, 0),
				gr.io_signature(1, 1, gr.sizeof_gr_complex))

        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose      = options.verbose      # turn verbose mode on/off
        self._tx_amplitude = options.tx_amplitude # digital amp sent to radio

        self.ofdm_tx = digital.ofdm_mod(options,
                                        msgq_limit=4,
                                        pad_for_usrp=False)

        self.amp = gr.multiply_const_cc(1)
        self.set_tx_amplitude(self._tx_amplitude)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

        # Create and setup transmit path flow graph
        self.connect(self.ofdm_tx, self.amp, self)

    def set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP
        @param: ampl 0 <= ampl < 1.0.  Try 0.10
        """
        self._tx_amplitude = max(0.0, min(ampl, 1))
        self.amp.set_k(self._tx_amplitude)
        
    def send_pkt(self, payload='', eof=False):
        """
        Calls the transmitter method to send a packet
        """
        return self.ofdm_tx.send_pkt(payload, eof)
        
    def add_options(normal, expert):
        """
        Adds transmitter-specific options to the Options Parser
        """
        normal.add_option("", "--tx-amplitude", type="eng_float",
                          default=0.1, metavar="AMPL",
                          help="set transmitter digital amplitude: 0 <= AMPL < 1.0 [default=%default]")
        normal.add_option("-W", "--bandwidth", type="eng_float",
                          default=500e3,
                          help="set symbol bandwidth [default=%default]")
        normal.add_option("-v", "--verbose", action="store_true",
                          default=False)
        expert.add_option("", "--log", action="store_true",
                          default=False,
                          help="Log all parts of flow graph to file (CAUTION: lots of data)")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

    def _print_verbage(self):
        """
        Prints information about the transmit path
        """
        print "Tx amplitude     %s" % (self._tx_amplitude)
        
