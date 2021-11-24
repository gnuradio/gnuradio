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
from gnuradio import digital
from gnuradio import analog

import copy
import sys

# /////////////////////////////////////////////////////////////////////////////
#                              receive path
# /////////////////////////////////////////////////////////////////////////////


class receive_path(gr.hier_block2):
    def __init__(self, rx_callback, options):

        gr.hier_block2.__init__(self, "receive_path",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0, 0, 0))

        # make a copy so we can destructively modify
        options = copy.copy(options)

        self._verbose = options.verbose
        self._log = options.log
        # this callback is fired when there's a packet available
        self._rx_callback = rx_callback

        # receiver
        self.ofdm_rx = digital.ofdm_demod(options,
                                          callback=self._rx_callback)

        # Carrier Sensing Blocks
        alpha = 0.001
        thresh = 30   # in dB, will have to adjust
        self.probe = analog.probe_avg_mag_sqrd_c(thresh, alpha)

        self.connect(self, self.ofdm_rx)
        self.connect(self.ofdm_rx, self.probe)

        # Display some information about the setup
        if self._verbose:
            self._print_verbage()

    def carrier_sensed(self):
        """
        Return True if we think carrier is present.
        """
        # return self.probe.level() > X
        return self.probe.unmuted()

    def carrier_threshold(self):
        """
        Return current setting in dB.
        """
        return self.probe.threshold()

    def set_carrier_threshold(self, threshold_in_db):
        """
        Set carrier threshold.

        Args:
            threshold_in_db: set detection threshold (float (dB))
        """
        self.probe.set_threshold(threshold_in_db)

    @staticmethod
    def add_options(normal, expert):
        """
        Adds receiver-specific options to the Options Parser
        """
        normal.add_option("-W", "--bandwidth", type="eng_float",
                          default=500e3,
                          help="set symbol bandwidth [default=%default]")
        normal.add_option("-v", "--verbose",
                          action="store_true", default=False)
        expert.add_option("", "--log", action="store_true", default=False,
                          help="Log all parts of flow graph to files (CAUTION: lots of data)")

    def _print_verbage(self):
        """
        Prints information about the receive path
        """
        pass
