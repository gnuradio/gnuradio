#!/usr/bin/env python
#
# Copyright 2006,2007,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, uhd, optfir, eng_notation, blks2, pager
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from string import split, join, printable
import sys

class app_top_block(gr.top_block):
    def __init__(self, options, queue):
	gr.top_block.__init__(self, "usrp_flex_all")

        if options.from_file is not None:
            self.u = gr.file_source(gr.sizeof_gr_complex, options.from_file)
            self.nchan = options.nchan
            if options.verbose:
                print "Reading samples from file", options.from_file
                print "User specified file contains", options.nchan, "25 KHz channels."

        else:
            # Set up USRP source
            self.u = uhd.usrp_source(device_addr=options.address, stream_args=uhd.stream_args('fc32'))

            # Tune daughterboard
            r = self.u.set_center_freq(options.freq+options.calibration, 0)
            if not r:
                frange = self.u.get_freq_range()
                sys.stderr.write(("\nRequested frequency (%f) out or range [%f, %f]\n") % \
                                     (freq, frange.start(), frange.stop()))
                sys.exit(1)

            if options.verbose:
                print "Tuned to center frequency", (options.freq+options.calibration)/1e6, "MHz"

            # if no gain was specified, use the mid-point in dB
            if options.rx_gain is None:
                grange = self.u.get_gain_range()
                options.rx_gain = float(grange.start()+grange.stop())/2.0
                print "\nNo gain specified."
                print "Setting gain to %f (from [%f, %f])" % \
                    (options.rx_gain, grange.start(), grange.stop())
        
            self.u.set_gain(options.rx_gain, 0)

            # Grab >=3 MHz of spectrum, evenly divisible by 25 KHz channels
            # (A UHD facility to get sample rate range and granularity would be useful)

            self.u.set_samp_rate(3.125e6) # Works if USRP is 100 Msps and can decimate by 32
            rate = self.u.get_samp_rate()

            if rate != 3.125e6:
                self.u.set_samp_rate(3.2e6) # Works if USRP is 64 Msps and can decimate by 20
                rate = self.u.get_samp_rate()
                if (rate != 3.2e6):
                    print "Unable to set required sample rate for >= 3MHz of 25 KHz channels."
                    sys.exit(1)
            
            self.nchan = int(rate/25e3)
            if options.verbose:
                print "\nReceiving", rate/1e6, "MHz of bandwidth containing", self.nchan, "baseband channels."
                    
        taps = gr.firdes.low_pass(1.0,
                                  1.0,
                                  1.0/self.nchan*0.4,
                                  1.0/self.nchan*0.1,
                                  gr.firdes.WIN_HANN)

        if options.verbose:
            print "Channel filter has", len(taps), "taps"

        self.bank = blks2.analysis_filterbank(self.nchan, taps)
        self.connect(self.u, self.bank)

        if options.log and options.from_file == None:
            src_sink = gr.file_sink(gr.sizeof_gr_complex, 'usrp.dat')
            self.connect(self.u, src_sink)

        mid_chan = int(self.nchan/2)
        for i in range(self.nchan):
	    if i < mid_chan:
		freq = 930.5e6+i*25e3
	    else:
		freq = 930.5e6-(self.nchan-i)*25e3

	    if (freq < 929.0e6 or freq > 932.0e6):
                self.connect((self.bank, i), gr.null_sink(gr.sizeof_gr_complex))
	    else:
            	self.connect((self.bank, i), pager.flex_demod(queue, freq, options.verbose, options.log))
                if options.log:
                    self.connect((self.bank, i), gr.file_sink(gr.sizeof_gr_complex, 'chan_'+'%3.3f'%(freq/1e6)+'.dat'))


def get_options():
    parser = OptionParser(option_class=eng_option)

    parser.add_option('-f', '--freq', type="eng_float", default=929.5125e6,
                      help="Set receive frequency to FREQ [default=%default]",
                      metavar="FREQ")
    parser.add_option("-a", "--address", type="string", default="addr=192.168.10.2",
                      help="Address of UHD device, [default=%default]")
    parser.add_option("-A", "--antenna", type="string", default=None,
                      help="select Rx Antenna where appropriate")
    parser.add_option("", "--rx-gain", type="eng_float", default=None,
                      help="set receive gain in dB (default is midpoint)")
    parser.add_option("-c",   "--calibration", type="eng_float", default=0.0,
                      help="set frequency offset to Hz", metavar="Hz")
    parser.add_option("-v", "--verbose", action="store_true", default=False)
    parser.add_option("-l", "--log", action="store_true", default=False,
                      help="log flowgraph to files (LOTS of data)")
    parser.add_option("-F", "--from-file", default=None,
                      help="read samples from file instead of USRP")
    parser.add_option("", "--nchan", type="int", default=None,
                      help="set to number of channels in capture file", metavar="nchan")

    (options, args) = parser.parse_args()

    if len(args) > 0:
	print "Run 'usrp_flex_all.py -h' for options."
	sys.exit(1)

    if options.nchan is None and options.from_file is not None:
        print "You must specify the number of baseband channels with --nchan when reading from a file"
        sys.exit(1)

    return (options, args)
   

def main():

    (options, args) = get_options()

    queue = gr.msg_queue()
    tb = app_top_block(options, queue)
    runner = pager.queue_runner(queue)

    try:
        tb.run()
    except KeyboardInterrupt:
        pass

    runner.end()
    
if __name__ == "__main__":
    main()
