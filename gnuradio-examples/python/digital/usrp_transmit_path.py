#
# Copyright 2009 Free Software Foundation, Inc.
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
import usrp_options
import transmit_path
from pick_bitrate import pick_tx_bitrate
from gnuradio import eng_notation

def add_freq_option(parser):
    """
    Hackery that has the -f / --freq option set both tx_freq and rx_freq
    """
    def freq_callback(option, opt_str, value, parser):
        parser.values.rx_freq = value
        parser.values.tx_freq = value

    if not parser.has_option('--freq'):
        parser.add_option('-f', '--freq', type="eng_float",
                          action="callback", callback=freq_callback,
                          help="set Tx and/or Rx frequency to FREQ [default=%default]",
                          metavar="FREQ")

def add_options(parser, expert):
    add_freq_option(parser)
    usrp_options.add_tx_options(parser)
    transmit_path.transmit_path.add_options(parser, expert)
    expert.add_option("", "--tx-freq", type="eng_float", default=None,
                          help="set transmit frequency to FREQ [default=%default]", metavar="FREQ")
    parser.add_option("-v", "--verbose", action="store_true", default=False)

class usrp_transmit_path(gr.hier_block2):
    def __init__(self, modulator_class, options):
        '''
        See below for what options should hold
        '''
        gr.hier_block2.__init__(self, "usrp_transmit_path",
                gr.io_signature(0, 0, 0),                    # Input signature
                gr.io_signature(0, 0, 0)) # Output signature
        if options.tx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --tx-freq FREQ must be specified\n")
            raise SystemExit
        tx_path = transmit_path.transmit_path(modulator_class, options)
        for attr in dir(tx_path): #forward the methods
            if not attr.startswith('_') and not hasattr(self, attr):
                setattr(self, attr, getattr(tx_path, attr))
        #setup usrp
        self._modulator_class = modulator_class
        self._setup_usrp_sink(options)
        #connect
        self.connect(tx_path, self.u)

    def _setup_usrp_sink(self, options):
        """
        Creates a USRP sink, determines the settings for best bitrate,
        and attaches to the transmitter's subdevice.
        """
        self.u = usrp_options.create_usrp_sink(options)
        dac_rate = self.u.dac_rate()
        if options.verbose:
            print 'USRP Sink:', self.u
        (self._bitrate, self._samples_per_symbol, self._interp) = \
                        pick_tx_bitrate(options.bitrate, self._modulator_class.bits_per_symbol(), \
                                        options.samples_per_symbol, options.interp, dac_rate, \
                                        self.u.get_interp_rates())

        self.u.set_interp(self._interp)
        self.u.set_auto_tr(True)

        if not self.u.set_center_freq(options.tx_freq):
            print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(options.tx_freq))
            raise ValueError, eng_notation.num_to_str(options.tx_freq)
