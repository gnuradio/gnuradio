#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation, usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from transmit_path import transmit_path
import sys

_dac_rate = 128e6

n2s = eng_notation.num_to_str

class tx_bpsk_block(gr.top_block):
    def __init__(self, options):    
	gr.top_block.__init__(self, "tx_mpsk")

        self._transmitter = transmit_path(options.sps,
                                          options.excess_bw,
                                          options.amplitude)

        if_rate = options.rate*options.sps
        interp = int(_dac_rate/if_rate)

        print "Modulation:", n2s(options.rate), "bits/sec"
        print "TX IF rate:", n2s(if_rate), "samples/sec"
        print "USRP interpolation:", interp
        print "DAC amplitude:", options.amplitude

        self._setup_usrp(options.which,
                         interp,
                         options.tx_subdev_spec,
                         options.freq)
	
	self.connect(self._transmitter, self._usrp)

        
    def _setup_usrp(self, which, interp, subdev_spec, freq):
        self._usrp = usrp.sink_c(which=which, interp_rate=interp)
	if subdev_spec is None:
	    subdev_spec = usrp.pick_tx_subdevice(self._usrp)
	self._usrp.set_mux(usrp.determine_tx_mux_value(self._usrp, subdev_spec))
	self._subdev = usrp.selected_subdev(self._usrp, subdev_spec)
	tr = usrp.tune(self._usrp, self._subdev._which, self._subdev, freq)
        if not (tr):
            print "Failed to tune to center frequency!"
        else:
            print "Center frequency:", n2s(freq)
        gain = float(self._subdev.gain_range()[1]) # Max TX gain
	self._subdev.set_gain(gain)
	self._subdev.set_enable(True)
        print "TX d'board:", self._subdev.side_and_name()
        

def get_options():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-w", "--which", type="int", default=0,
                      help="select which USRP (0, 1, ...) default is %default",
                      metavar="NUM")
    parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                      help="select USRP Tx side A or B (default=first one with a daughterboard)")
    parser.add_option("-f", "--freq", type="eng_float", default=None,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-a", "--amplitude", type="eng_float", default=2000,
                      help="set Tx amplitude (0-32767) (default=%default)")
    parser.add_option("-r", "--rate", type="eng_float", default=250e3,
                      help="Select modulation symbol rate (default=%default)")
    parser.add_option("", "--sps", type="int", default=2,
                      help="Select samples per symbol (default=%default)")
    parser.add_option("", "--excess-bw", type="eng_float", default=0.35,
                      help="Select RRC excess bandwidth (default=%default)")
		      
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)
	
    if options.freq == None:
        print "Must supply frequency as -f or --freq"
        sys.exit(1)

    return (options, args)

if __name__ == "__main__":
    (options, args) = get_options()
    
    tb = tx_bpsk_block(options)

    try:
        tb.run()
    except KeyboardInterrupt:
        pass
