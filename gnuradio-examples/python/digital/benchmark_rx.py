#!/usr/bin/env python
#
# Copyright 2005,2006,2007,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, modulation_utils
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random
import struct
import sys

# from current dir
from receive_path import receive_path
from pick_bitrate import pick_rx_bitrate
import usrp_options

#import os
#print os.getpid()
#raw_input('Attach and press enter: ')

class my_top_block(gr.top_block):
    def __init__(self, demodulator, rx_callback, options):
        gr.top_block.__init__(self)

        self._rx_freq            = options.rx_freq         # receiver's center frequency
        self._rx_gain            = options.rx_gain         # receiver's gain
        self._decim              = options.decim           # Decimating rate for the USRP (prelim)
        self._bitrate            = options.bitrate
        self._samples_per_symbol = options.samples_per_symbol
        self._demod_class        = demodulator

        if self._rx_freq is None:
            sys.stderr.write("-f FREQ or --freq FREQ or --rx-freq FREQ must be specified\n")
            raise SystemExit

        # Set up USRP source
        self._setup_usrp_source(options)
        ok = self.set_freq(self._rx_freq)
        if not ok:
            print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(self._rx_freq))
            raise ValueError, eng_notation.num_to_str(self._rx_freq)

        self.set_gain(options.rx_gain)
        #self.set_auto_tr(True)                 # enable Auto Transmit/Receive switching

        # Set up receive path
        self.rxpath = receive_path(demodulator, rx_callback, options) 

        self.connect(self.u, self.rxpath)
        
    def _setup_usrp_source(self, options):
        self.u = usrp_options.create_usrp_source(options)
        adc_rate = self.u.adc_rate()

        self.u.set_decim(self._decim)

        (self._bitrate, self._samples_per_symbol, self._decim) = \
                        pick_rx_bitrate(self._bitrate, self._demod_class.bits_per_symbol(), \
                                        self._samples_per_symbol, self._decim, adc_rate,  \
                                        self.u.get_decim_rates())

        self.u.set_decim(self._decim)


    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital up converter.
        """
        return self.u.set_center_freq(target_freq)

    def set_gain(self, gain):
        """
        Sets the analog gain in the USRP
        """
        if gain is None:
            r = self.u.gain_range()
            gain = (r[0] + r[1])/2               # set gain to midpoint
        self.gain = gain
        return self.u.set_gain(gain)

    def set_auto_tr(self, enable):
        return self.subdev.set_auto_tr(enable)

    def decim(self):
        return self._decim

    def add_options(normal, expert):
        """
        Adds usrp-specific options to the Options Parser
        """
        add_freq_option(normal)
        normal.add_option("-v", "--verbose", action="store_true", default=False)

        expert.add_option("", "--rx-freq", type="eng_float", default=None,
                          help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
   

    # Make a static method to call before instantiation
    add_options = staticmethod(add_options)


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


# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

global n_rcvd, n_right

def main():
    global n_rcvd, n_right

    n_rcvd = 0
    n_right = 0
    
    def rx_callback(ok, payload):
        global n_rcvd, n_right
        (pktno,) = struct.unpack('!H', payload[0:2])
        n_rcvd += 1
        if ok:
            n_right += 1

        print "ok = %5s  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
            ok, pktno, n_rcvd, n_right)


    demods = modulation_utils.type_1_demods()

    # Create Options Parser:
    parser = OptionParser (option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")

    parser.add_option("-m", "--modulation", type="choice", choices=demods.keys(), 
                      default='gmsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(demods.keys()),))

    my_top_block.add_options(parser, expert_grp)
    receive_path.add_options(parser, expert_grp)
    usrp_options.add_rx_options(parser)

    for mod in demods.values():
        mod.add_options(expert_grp)

    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.rx_freq is None:
        sys.stderr.write("You must specify -f FREQ or --freq FREQ\n")
        parser.print_help(sys.stderr)
        sys.exit(1)


    # build the graph
    tb = my_top_block(demods[options.modulation], rx_callback, options)

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: Failed to enable realtime scheduling."

    tb.start()        # start flow graph
    tb.wait()         # wait for it to finish

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
