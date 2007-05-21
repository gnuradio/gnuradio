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

FR_MODE = usrp.FR_USER_0
bmFR_MODE_RESET = 1 << 0	# bit 0: active high reset
bmFR_MODE_TX    = 1 << 1	# bit 1: enable transmitter
bmFR_MODE_RX    = 1 << 2	# bit 2: enable receiver
bmFR_MODE_LP    = 1 << 3	# bit 3: enable digital loopback

FR_DEGREE = usrp.FR_USER_1

class sounder:
    def __init__(self, options):
	self._options = options
	self._mode = 0
	
	self._u = None
	self._trans = None
	self._rcvr = None
	self._transmitting = False
	self._receiving = False
		
	if options.transmit:
	    print "Creating sounder transmitter."
	    self._trans = usrp.sink_s(fpga_filename='usrp_sounder.rbf')
            self._trans_subdev_spec = usrp.pick_tx_subdevice(self._trans)
	    self._trans_subdev = usrp.selected_subdev(self._trans, self._trans_subdev_spec)
	    self._trans.start()
	    self._u = self._trans
	        
	if options.receive:
	    print "Creating sounder receiver."
            self._fg = gr.flow_graph()
    	    self._rcvr = usrp.source_s(fpga_filename='usrp_sounder.rbf', decim_rate=128)
	    self._rcvr_subdev_spec = usrp.pick_rx_subdevice(self._rcvr)
	    self._rcvr_subdev = usrp.selected_subdev(self._rcvr, self._rcvr_subdev_spec)
	    self._sink = gr.file_sink(gr.sizeof_short, "output.dat")

	    if options.samples >= 0:
		self._head = gr.head(gr.sizeof_short, options.samples*gr.sizeof_short)
		self._fg.connect(self._rcvr, self._head, self._sink)
	    else:
		self._fg.connect(self._rcvr, self._sink)
	    self._u = self._rcvr # either receiver or transmitter object will do
	
	self.set_reset(True)
	self.set_freq(options.frequency)
	self.set_degree(options.degree)
	self.set_loopback(options.loopback)	
	self.set_reset(False)
		
    def set_freq(self, frequency):
	print "Setting center frequency to", n2s(frequency)
	if self._rcvr:
	    self._rcvr.tune(0, self._rcvr_subdev, frequency)
	
	if self._trans:
	    self._trans.tune(0, self._trans_subdev, frequency)

    def set_degree(self, degree):
	print "Setting PN code degree to", degree
        self._u._write_fpga_reg(FR_DEGREE, degree);
	    
    def _write_mode(self):
	print "Writing mode register with:", hex(self._mode)
        self._u._write_fpga_reg(FR_MODE, self._mode)

    def enable_tx(self, value):
	if value:
	    print "Enabling transmitter."
	    self._mode |= bmFR_MODE_TX
	    self._transmitting = True
	else:
	    print "Disabling transmitter."
	    self._mode &= ~bmFR_MODE_TX
	self._write_mode()
		    
    def enable_rx(self, value):
	if value:
	    print "Starting receiver flow graph."
	    self._mode |= bmFR_MODE_RX
	    self._write_mode()
	    self._fg.start()
	    self._receiving = True
	    if self._options.samples >= 0:
		self._fg.wait()
	else:
	    print "Stopping receiver flow graph."
	    if self._options.samples < 0:
		self._fg.stop()
		print "Waiting for threads..."
		self._fg.wait()
		print "Receiver flow graph stopped."
	    self._mode &= ~bmFR_MODE_RX
	    self._write_mode()
	    self._receiving = False
	        
    def set_loopback(self, value):
	if value:
	    print "Enabling digital loopback."
	    self._mode |= bmFR_MODE_LP
	else:
	    print "Disabling digital loopback."
	    self._mode &= ~bmFR_MODE_LP
	self._write_mode()

    def set_reset(self, value):
	if value:
	    print "Asserting reset."
	    self._mode |= bmFR_MODE_RESET
	else:
	    print "De-asserting reset."
	    self._mode &= ~bmFR_MODE_RESET
	self._write_mode()

    def __del__(self):
	if self._transmitting:
	    self.enable_tx(False)
	    
	if self._receiving:
	    self.enable_rx(False)
	    
# ------------------------------------------------------------------------------
    
def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float", default=0.0,
                      help="set frequency to FREQ in Hz, default is %default", metavar="FREQ")

    parser.add_option("-d", "--degree", type="int", default=16,
                      help="set souding sequence degree (len=2^degree-1), default is %default")

    parser.add_option("-t", "--transmit", action="store_true", default=False,
                      help="enable sounding transmitter")

    parser.add_option("-r", "--receive", action="store_true", default=False,
                      help="enable sounding receiver")

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
    
    s = sounder(options)

    if options.transmit:
	s.enable_tx(True)

    if options.receive:
	s.enable_rx(True)

    if options.samples < 0:
        raw_input("Press enter to exit.")
	
if __name__ == "__main__":
    main()
