#!/usr/bin/env python

#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, usrp, optfir, eng_notation, pager
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import time, os, sys
from string import split, join

"""
This example application demonstrates receiving and demodulating the
FLEX pager protocol.

The following are required command line parameters:

-f FREQ	    USRP receive frequency

The following are optional command line parameters:

-R SUBDEV   Daughter board specification, defaults to first found
-F FILE     Read samples from a file instead of USRP.
-c FREQ     Calibration offset.  Gets added to receive frequency.
            Defaults to 0.0 Hz.
-g GAIN     Daughterboard gain setting. Defaults to mid-range.
-l          Log flow graph to files (LOTS of data)
-v          Verbose output

Once the program is running, ctrl-break (Ctrl-C) stops operation.
"""

class app_top_block(gr.top_block):
    def __init__(self, options, queue):
        gr.top_block.__init__(self, "usrp_flex")
        self.options = options
	self.offset = 0.0
	self.adj_time = time.time()
	self.verbose = options.verbose
			
	if options.from_file is None:
            # Set up USRP source with specified RX daughterboard
            self.src = usrp.source_c()
            if options.rx_subdev_spec == None:
                options.rx_subdev_spec = usrp.pick_rx_subdevice(self.src)
            self.subdev = usrp.selected_subdev(self.src, options.rx_subdev_spec)
            self.src.set_mux(usrp.determine_rx_mux_value(self.src, options.rx_subdev_spec))

            # Grab 250 KHz of spectrum (sample rate becomes 250 ksps complex)
            self.src.set_decim_rate(256)
	    	    
            # If no gain specified, set to midrange
            if options.gain is None:
                g = self.subdev.gain_range()
                options.gain = (g[0]+g[1])/2.0
            self.subdev.set_gain(options.gain)

            # Tune daughterboard
            actual_frequency = options.frequency+options.calibration
            tune_result = usrp.tune(self.src, 0, self.subdev, actual_frequency)
            if not tune_result:
                sys.stderr.write("Failed to set center frequency to "+`actual_frequency`+"\n")
                sys.exit(1)

            if options.verbose:
                print "Using RX daughterboard", self.subdev.side_and_name()
                print "USRP gain is", options.gain
                print "USRP tuned to", actual_frequency
            
        else:
            # Use supplied file as source of samples
            self.src = gr.file_source(gr.sizeof_gr_complex, options.from_file)
            if options.verbose:
                print "Reading samples from", options.from_file
	    
        if options.log and not options.from_file:
            usrp_sink = gr.file_sink(gr.sizeof_gr_complex, 'usrp.dat')
            self.connect(self.src, usrp_sink)

        # Set up 22KHz-wide bandpass about center frequency. Decimate by 10
        # to get channel rate of 25Ksps
        taps = optfir.low_pass(1.0,   # Filter gain
                               250e3, # Sample rate
                               11000, # One-sided modulation bandwidth
                               12500, # One-sided channel bandwidth
                               0.1,   # Passband ripple
                               60)    # Stopband attenuation
	
	if options.verbose:
	    print "Channel filter has", len(taps), "taps."

        self.chan = gr.freq_xlating_fir_filter_ccf(10,    # Decimation rate
                                              taps,  # Filter taps
                                              0.0,   # Offset frequency
                                              250e3) # Sample rate

	if options.log:
	    chan_sink = gr.file_sink(gr.sizeof_gr_complex, 'chan.dat')
	    self.connect(self.chan, chan_sink)

        # FLEX protocol demodulator
        self.flex = pager.flex_demod(queue, options.frequency, options.verbose, options.log)

        self.connect(self.src, self.chan, self.flex)

    def freq_offset(self):
	return self.flex.dc_offset()*1600

    def adjust_freq(self):
	if time.time() - self.adj_time > 1.6:	# Only do it once per FLEX frame
	    self.adj_time = time.time()
	    self.offset -= self.freq_offset()
	    self.chan.set_center_freq(self.offset)
	    if self.verbose:
		print "Channel frequency offset (Hz):", int(self.offset)
	    		
def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float", default=None,
                      help="set receive frequency to Hz", metavar="Hz")
    parser.add_option("-R", "--rx-subdev-spec", type="subdev",
                      help="select USRP Rx side A or B", metavar="SUBDEV")
    parser.add_option("-c",   "--calibration", type="eng_float", default=0.0,
                      help="set frequency offset to Hz", metavar="Hz")
    parser.add_option("-g", "--gain", type="int", default=None,
                      help="set RF gain", metavar="dB")
    parser.add_option("-l", "--log", action="store_true", default=False,
                      help="log flowgraph to files (LOTS of data)")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="display debug output")
    parser.add_option("-F", "--from-file", default=None,
                      help="read samples from file instead of USRP")
    (options, args) = parser.parse_args()

    if len(args) > 0 or (options.frequency == None and options.from_file == None):
	print "Run 'usrp_flex.py -h' for options."
	sys.exit(1)

    if options.frequency == None:
	options.frequency = 0.0

    # Flow graph emits pages into message queue
    queue = gr.msg_queue()
    tb = app_top_block(options, queue)
    
    try:
        tb.start()
	while 1:
	    if not queue.empty_p():
		msg = queue.delete_head() # Blocking read
		page = join(split(msg.to_string(), chr(128)), '|')
		disp = []
		for n in range(len(page)):
		    if ord(page[n]) < 32:
			disp.append('.')
		    else:
			disp.append(page[n])
		print join(disp, '')
		tb.adjust_freq()
										
	    else:
		time.sleep(1)

    except KeyboardInterrupt:
        tb.stop()

if __name__ == "__main__":
    main()
