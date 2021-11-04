#
# Copyright 2005,2006,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr
from gnuradio import eng_notation
from gnuradio import blocks
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

        # make a copy so we can destructively modify
        options = copy.copy(options)

        self._verbose = options.verbose      # turn verbose mode on/off
        self._tx_amplitude = options.tx_amplitude  # digital amp sent to radio

        self.ofdm_tx = digital.ofdm_mod(options,
                                        msgq_limit=4,
                                        pad_for_usrp=False)

        self.amp = blocks.multiply_const_cc(1)
        self.set_tx_amplitude(self._tx_amplitude)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

        # Create and setup transmit path flow graph
        self.connect(self.ofdm_tx, self.amp, self)

    def set_tx_amplitude(self, ampl):
        """
        Sets the transmit amplitude sent to the USRP

        Args:
            : ampl 0 <= ampl < 1.0.  Try 0.10
        """
        self._tx_amplitude = max(0.0, min(ampl, 1))
        self.amp.set_k(self._tx_amplitude)

    def send_pkt(self, payload='', eof=False):
        """
        Calls the transmitter method to send a packet
        """
        return self.ofdm_tx.send_pkt(payload, eof)

    @staticmethod
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

    def _print_verbage(self):
        """
        Prints information about the transmit path
        """
        print("Tx amplitude     %s" % (self._tx_amplitude))
