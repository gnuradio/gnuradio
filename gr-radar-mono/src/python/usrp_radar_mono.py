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
from gnuradio.radar_mono import radar
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

n2s = eng_notation.num_to_str

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float", default=0.0,
                      help="set transmitter center frequency to FREQ in Hz, default is %default", metavar="FREQ")
    parser.add_option("-w", "--chirp-width", type="eng_float", default=32e6,
                      help="set LFM chirp bandwidth in Hz, default is %default", metavar="FREQ")
    parser.add_option("-a", "--amplitude", type="eng_float", default=15,
                      help="set waveform amplitude in % full scale, default is %default,")
    parser.add_option("",   "--ton", type="eng_float", default=5e-6,
		      help="set pulse on period in seconds, default is %default,")
    parser.add_option("",   "--tsw", type="eng_float", default=406.25e-9,
		      help="set transmitter switching period in seconds, default is %default,")
    parser.add_option("",   "--tlook", type="eng_float", default=5e-6,
		      help="set receiver look time in seconds, default is %default,")
    parser.add_option("",   "--prf", type="eng_float", default=10e3,
		      help="set pulse repetition frequency in Hz, default is %default,")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="enable verbose output, default is disabled")
    parser.add_option("-D", "--debug", action="store_true", default=False,
                      help="enable debugging output, default is disabled")

    # NOT IMPLEMENTED
    #parser.add_option("-g", "--gain", type="eng_float", default=None,
    #                  help="set gain in dB (default is midpoint)")
    #parser.add_option("-l", "--loopback", action="store_true", default=False,
    #                  help="enable digital loopback, default is disabled")
    #parser.add_option("-F", "--filename", default=None,
    #                  help="log received echos to file")
		      
    (options, args) = parser.parse_args()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    """
    if options.filename == None:
        print "Must supply filename for logging received data."
        sys.exit(1)
    else:
        if options.verbose:
            print "Logging echo records to file: ", options.filename
    """
        
    msgq = gr.msg_queue()
    s = radar(msgq=msgq,verbose=options.verbose,debug=options.debug)

    s.set_ton(options.ton)
    s.set_tsw(options.tsw)
    s.set_tlook(options.tlook)
    s.set_prf(options.prf)
    s.set_amplitude(options.amplitude)
    s.set_freq(options.frequency, options.chirp_width)

    s.start()

    """
    f = open(options.filename, "wb")
    print "Enter CTRL-C to stop."
    try:
        while (1):
            msg = msgq.delete_head()
            if msg.type() == 1:
                break
            rec = msg.to_string()
            if options.debug:
                print "Received echo vector of length", len(rec)
		f.write(rec)
		
    except KeyboardInterrupt:
        pass
    """

    raw_input("Press enter to stop transmitting.")
    s.stop()
        
if __name__ == "__main__":
    main()
