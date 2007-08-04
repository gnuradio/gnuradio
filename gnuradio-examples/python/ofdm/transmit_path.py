#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, blks
from gnuradio import usrp
from gnuradio import eng_notation

import copy
import sys

# /////////////////////////////////////////////////////////////////////////////
#                              transmit path
# /////////////////////////////////////////////////////////////////////////////

class transmit_path(gr.hier_block): 
    def __init__(self, fg, options):
        '''
        See below for what options should hold
        '''

        options = copy.copy(options)    # make a copy so we can destructively modify

        self._verbose      = options.verbose
        self._tx_amplitude = options.tx_amplitude    # digital amplitude sent to USRP

        self.ofdm_tx = \
                     blks.ofdm_mod(fg, options, msgq_limit=4, pad_for_usrp=False)

        self.amp = gr.multiply_const_cc(1)
        self.set_tx_amplitude(self._tx_amplitude)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

        # Create and setup transmit path flow graph
        fg.connect(self.ofdm_tx, self.amp)
        gr.hier_block.__init__(self, fg, None, self.amp)

    def set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP
        @param: ampl 0 <= ampl < 32768.  Try 8000
        """
        self._tx_amplitude = max(0.0, min(ampl, 32767.0))
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
        normal.add_option("", "--tx-amplitude", type="eng_float", default=200, metavar="AMPL",
                          help="set transmitter digital amplitude: 0 <= AMPL < 32768 [default=%default]")
        normal.add_option("-v", "--verbose", action="store_true", default=False)
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to file (CAUTION: lots of data)")

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)

    def _print_verbage(self):
        """
        Prints information about the transmit path
        """
        print "Tx amplitude     %s"    % (self._tx_amplitude)
        
