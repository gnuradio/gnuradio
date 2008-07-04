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

from gnuradio import gr
from gnuradio import msdd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import time

class benchmark_msdd6000(gr.top_block):
    def __init__(self, address, options):
        gr.top_block.__init__(self)

        # Extract the initial options
        self.frequency = options.frequency
        self.filename  = options.filename
        self.decim     = options.decim
        self.gain      = options.gain
        self.address   = address

        # Set up and initialize the MSDD receiver
        self.port = 10001 # required port
        self.src = msdd.source_c(0, 1, self.address, self.port)
        self.src.set_decim_rate(self.decim)
        self.src.set_desired_packet_size(0, 1460)
        self.src.set_pga(0, self.gain)
        self.src.set_rx_freq(0, self.frequency)

        # Display some info
        print "Min PGA: ", self.src.pga_min()
        print "Max PGA: ", self.src.pga_max()
        print "PGA:     ", self.src.pga(0)
        print "Decim:   ", self.src.decim_rate()
        print "Freq:    ", self.src.rx_freq(0)
        
        # Build a file sink to save the info for post analysis
        self.snk = gr.file_sink(gr.sizeof_gr_complex, self.filename)

        # Connect the reciever source to file sink
        self.connect(self.src, self.snk)
        
def main():
    ''' This is a simple little script to play with retunning of the MSDD board.
    You can cycle through frequencies or the attenuation of the board here.
    '''

    usage="%prog: [options] host_address"
    parser = OptionParser(usage=usage, option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-f", "--frequency", type="eng_float", default=100e6,
                      help="set frequency (Hz) [default=%default]")
    parser.add_option("-d", "--decim", type="int", default=256,
                      help="set decimation rate [default=%default]")
    parser.add_option("-g", "--gain", type="int", default=32,
                      help="set receiver gain (dB) [default=%default]")
    parser.add_option("-F", "--filename", type="string", default="output.dat",
                      help="set output filename [default=%default]")
    (options, args) = parser.parse_args ()
    host_address = args[0]

    # Set up benchmark system that simply connects the MSDD source to a file sink
    tb = benchmark_msdd6000(host_address, options)
    tb.start() # start it here

    # Adjust your parameters here. Use the time.sleep(x) function to set a wait period
    # between adjusting the parameter.
    for i in range(7):
        time.sleep(0.5)
        if 0:
            freq = (tb.src.rx_freq(0) + 1) * 1e6
            tb.src.set_rx_freq(0, freq)
            print "Setting frequency: ", freq
        if 1:
            pga = tb.src.pga(0)+10
            tb.src.set_pga(0, pga)
            print "Setting PGA: ", pga

    tb.stop()  # stop the radio

if __name__ == '__main__':
    main()
