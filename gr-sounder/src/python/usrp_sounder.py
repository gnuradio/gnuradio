#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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
from gnuradio.sounder import sounder
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import numpy
import sys

n2s = eng_notation.num_to_str

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                      help="select USRP Rx side A or B")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set gain in dB (default is midpoint)")
    parser.add_option("-f", "--frequency", type="eng_float", default=0.0,
                      help="set frequency to FREQ in Hz, default is %default", metavar="FREQ")
    parser.add_option("-d", "--degree", type="int", default=12,
                      help="set sounding sequence degree (2-12), default is %default,")
    parser.add_option("-a", "--amplitude", type="int", default=4096,
                      help="set waveform amplitude, default is %default,")
    parser.add_option("-t", "--transmit", action="store_true", default=False,
                      help="enable sounding transmitter")
    parser.add_option("-r", "--receive", action="store_true", default=False,
                      help="enable sounding receiver")
    parser.add_option("-l", "--loopback", action="store_true", default=False,
                      help="enable digital loopback, default is disabled")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="enable verbose output, default is disabled")
    parser.add_option("-D", "--debug", action="store_true", default=False,
                      help="enable debugging output, default is disabled")
    parser.add_option("-F", "--filename", default=None,
		      help="log received impulse responses to file")
    parser.add_option("", "--alpha", type="eng_float", default=1.0,
		      help="smoothing factor (0.0-1.0), default is %default (none)")
		      
    (options, args) = parser.parse_args()

    if len(args) != 0 or not (options.transmit | options.receive):
        parser.print_help()
        sys.exit(1)

    if options.receive and (options.filename == None):
	print "Must supply filename when receiving."
	sys.exit(1)

    if options.degree > 12 or options.degree < 2:
	print "PN code degree must be between 2 and 12"
	sys.exit(1)

    length = int(2**options.degree-1)
    if options.verbose:
        print "Using PN code degree of", options.degree, "length", length
	if options.loopback == False:
            print "Sounding frequency range is", n2s(options.frequency-16e6), "to", n2s(options.frequency+16e6)
	if options.filename != None:
	    print "Logging impulse records to file: ", options.filename
	    
    msgq = gr.msg_queue()
    s = sounder(transmit=options.transmit,receive=options.receive,
                loopback=options.loopback,rx_subdev_spec=options.rx_subdev_spec,
                frequency=options.frequency,rx_gain=options.gain,
                degree=options.degree,length=length,alpha=options.alpha,
                msgq=msgq,verbose=options.verbose,ampl=options.amplitude,
                debug=options.debug)
    s.start()

    if options.receive:
	f = open(options.filename, "wb")
	print "Enter CTRL-C to stop."
        try:
            while (1):
                msg = msgq.delete_head()
		if msg.type() == 1:
		    break
		rec = msg.to_string()[:length*gr.sizeof_gr_complex]
		if options.debug:
		    print "Received impulse vector of length", len(rec)

		f.write(rec)
		
        except KeyboardInterrupt:
            pass
    else:
        if options.transmit:
            raw_input("Press return to exit.")
        
if __name__ == "__main__":
    main()
