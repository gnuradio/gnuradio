#!/usr/bin/env python
#
# Copyright 2010,2011 Free Software Foundation, Inc.
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
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random, math, sys

class my_top_block(gr.top_block):
    def __init__(self, ifile, ofile, options):
        gr.top_block.__init__(self)

        SNR = 10.0**(options.snr/10.0)
        time_offset = options.time_offset
        phase_offset = options.phase_offset*(math.pi/180.0)

        # calculate noise voltage from SNR
        power_in_signal = abs(options.tx_amplitude)**2
        noise_power = power_in_signal/SNR
        noise_voltage = math.sqrt(noise_power)
        print "Noise voltage: ", noise_voltage

        frequency_offset = options.frequency_offset / options.fft_length

        self.src = gr.file_source(gr.sizeof_gr_complex, ifile)
        #self.throttle = gr.throttle(gr.sizeof_gr_complex, options.sample_rate)
        self.channel = gr.channel_model(noise_voltage, frequency_offset,
                                        time_offset, noise_seed=-random.randint(0,100000))
        self.phase = gr.multiply_const_cc(complex(math.cos(phase_offset),
                                                  math.sin(phase_offset)))
        self.snk = gr.file_sink(gr.sizeof_gr_complex, ofile)

        self.connect(self.src, self.channel, self.phase, self.snk)
        

# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():
    # Create Options Parser:
    usage = "benchmack_add_channel.py [options] <input file> <output file>"
    parser = OptionParser (usage=usage, option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-n", "--snr", type="eng_float", default=30,
                      help="set the SNR of the channel in dB [default=%default]")
    parser.add_option("", "--seed", action="store_true", default=False,
                      help="use a random seed for AWGN noise [default=%default]")
    parser.add_option("-f", "--frequency-offset", type="eng_float", default=0,
                      help="set frequency offset introduced by channel [default=%default]")
    parser.add_option("-t", "--time-offset", type="eng_float", default=1.0,
                      help="set timing offset between Tx and Rx [default=%default]")
    parser.add_option("-p", "--phase-offset", type="eng_float", default=0,
                      help="set phase offset (in degrees) between Tx and Rx [default=%default]")
    parser.add_option("-m", "--use-multipath", action="store_true", default=False,
                      help="Use a multipath channel [default=%default]")
    parser.add_option("", "--fft-length", type="intx", default=None,
                      help="set the number of FFT bins [default=%default]")
    parser.add_option("", "--tx-amplitude", type="eng_float",
                      default=1.0,
                      help="tell the simulator the signal amplitude [default=%default]")

    (options, args) = parser.parse_args ()

    if len(args) != 2:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.fft_length is None:
        sys.stderr.write("Please enter the FFT length of the OFDM signal.\n")
        sys.exit(1)

    ifile = args[0]
    ofile = args[1]
        
    # build the graph
    tb = my_top_block(ifile, ofile, options)

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
