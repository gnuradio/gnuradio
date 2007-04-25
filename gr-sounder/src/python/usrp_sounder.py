#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys, time

n2s = eng_notation.num_to_str

# Set to 0 for 32 MHz tx clock, 1 for 64 MHz tx clock
# Must match config.vh in FPGA code
TX_RATE_MAX = 0
_tx_freq_divisor = 32e6*(TX_RATE_MAX+1)

class sounder_tx:
    def __init__(self, frequency, degree, loopback):
	self.trans = usrp.sink_s(fpga_filename='usrp_sounder.rbf')
        self.subdev_spec = usrp.pick_tx_subdevice(self.trans)
        self.subdev = usrp.selected_subdev(self.trans, self.subdev_spec)
        self.trans.tune(0, self.subdev, frequency)
        self.set_degree(degree);
        self.set_loopback(loopback)
            
    def turn_on(self):
	self.trans.start()
	
    def turn_off(self):
	self.trans.stop()

    def set_degree(self, value):
        return self.trans._write_fpga_reg(usrp.FR_USER_0, value);

    def set_loopback(self, value):
        return self.trans._write_fpga_reg(usrp.FR_USER_1, value==True);

class sounder_rx:
    def __init__(self, frequency, degree, samples):
        self.fg = gr.flow_graph()
	self.rcvr = usrp.source_s(fpga_filename='usrp_sounder.rbf', decim_rate=8)
        self.subdev_spec = usrp.pick_rx_subdevice(self.rcvr)
        self.subdev = usrp.selected_subdev(self.rcvr, self.subdev_spec)
        self.rcvr.tune(0, self.subdev, frequency)
        self.set_degree(degree);
	self.sink = gr.file_sink(gr.sizeof_short, "output.dat")

	if samples >= 0:
	    self.head = gr.head(gr.sizeof_short, 2*samples*gr.sizeof_short)
	    self.fg.connect(self.rcvr, self.head, self.sink)
	else:
	    self.fg.connect(self.rcvr, self.sink)

    def receive(self):
	self.fg.run()
	
    def set_degree(self, value):
        return self.rcvr._write_fpga_reg(usrp.FR_USER_0, value);

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float", default=0.0,
                      help="set frequency to FREQ in Hz, default is %default", metavar="FREQ")

    parser.add_option("-t", "--transmit", action="store_true", default=False,
                      help="enable sounding transmitter")

    parser.add_option("-r", "--receive", action="store_true", default=False,
                      help="enable sounding receiver")

    parser.add_option("-d", "--degree", type="int", default=16,
                      help="set souding sequence degree (len=2^degree-1), default is %default")

    parser.add_option("-n", "--samples", type="int", default=-1,
                      help="number of samples to capture on receive, default is infinite")

    parser.add_option("-l", "--loopback", action="store_true", default=False,
                      help="enable digital loopback, default is disabled")

    (options, args) = parser.parse_args()

    if len(args) != 0 or not (options.transmit | options.receive):
        parser.print_help()
        sys.exit(1)

    print "Using PN code degree of", options.degree, "length", 2**options.degree-1
    print "Sounding frequency range is", n2s(options.frequency-16e6), "to", n2s(options.frequency+16e6)
    
    if (options.transmit):
	print "Enabling sounder transmitter."
	if (options.loopback):
	    print "Enabling digital loopback."
        tx = sounder_tx(options.frequency, options.degree, options.loopback)
	tx.turn_on()
	
    try:
        if (options.receive):
    	    print "Enabling sounder receiver."
	    rx = sounder_rx(options.frequency, options.degree, options.samples)
	    rx.receive()
	else:
	    if (options.transmit):
		while (True): time.sleep(1.0)

    except KeyboardInterrupt:
	if (options.transmit):
	    tx.turn_off()

	
if __name__ == "__main__":
    main()
