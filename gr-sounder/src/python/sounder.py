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

from gnuradio import gr, usrp
from gnuradio import eng_notation

n2s = eng_notation.num_to_str

FR_MODE = usrp.FR_USER_0
bmFR_MODE_RESET = 1 << 0	# bit 0: active high reset
bmFR_MODE_TX    = 1 << 1	# bit 1: enable transmitter
bmFR_MODE_RX    = 1 << 2	# bit 2: enable receiver
bmFR_MODE_LP    = 1 << 3	# bit 3: enable digital loopback

FR_DEGREE = usrp.FR_USER_1
FR_AMPL = usrp.FR_USER_2

def pick_subdevice(u):
    """
    The user didn't specify a subdevice on the command line.
    If there's a daughterboard on A, select A.
    If there's a daughterboard on B, select B.
    Otherwise, select A.
    """
    if u.db[0][0].dbid() >= 0:       # dbid is < 0 if there's no d'board or a problem
        return (0, 0)
    if u.db[1][0].dbid() >= 0:
        return (1, 0)
    return (0, 0)

class sounder_tx:
    def __init__(self, loopback=False,ampl=4096,verbose=False,debug=False):
	self._loopback=loopback
        self._amplitude = ampl
	self._verbose = verbose
	self._debug = debug
        self._u = usrp.sink_s(fpga_filename='usrp_sounder.rbf')
	if not self._loopback:
            self._subdev_spec = usrp.pick_tx_subdevice(self._u)
            self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
            if self._verbose:
                print "Using", self._subdev.name(), "for sounder transmitter."            
        self.set_amplitude(ampl)
	if not self._loopback:
          self._subdev.set_lo_offset(0.0)
        self._u.start()
	if not self._loopback:
	  self._subdev.set_enable(True)
	    
    def tune(self, frequency):
        if self._verbose:
            print "Setting transmitter frequency to", n2s(frequency)
        result = self._u.tune(0, self._subdev, frequency)
        if result == False:
            raise RuntimeError("Failed to set transmitter frequency.")

    def set_amplitude(self, ampl):
	self._amplitude = ampl
        if self._debug:
            print "Writing amplitude register with:", hex(self._mode)
        self._u._write_fpga_reg(FR_AMPL, self._amplitude)

class sounder_rx:
    def __init__(self,subdev_spec=None,gain=None,length=1,alpha=1.0,msgq=None,loopback=False,verbose=False,debug=False):
	self._subdev_spec = subdev_spec
        self._gain = gain
        self._length = length
        self._alpha = alpha
        self._msgq = msgq
	self._loopback = loopback
	self._verbose = verbose
	self._debug = debug
		
        self._fg = gr.flow_graph()
        self._u = usrp.source_c(fpga_filename='usrp_sounder.rbf')
	if not self._loopback:
            if self._subdev_spec == None:
                self._subdev_spec = pick_subdevice(self._u)
            self._u.set_mux(usrp.determine_rx_mux_value(self._u, self._subdev_spec))
            self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
            if self._verbose:
	        print "Using", self._subdev.name(), "for sounder receiver."

        self.set_gain(self._gain)
        self._vblen = gr.sizeof_gr_complex*self._length
	if self._debug:
            print "Generating impulse vectors of length", self._length, "byte length", self._vblen
            
        self._s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self._length)
	if self._verbose:
	    print "Using smoothing alpha of", self._alpha
        self._lpf = gr.single_pole_iir_filter_cc(self._alpha, self._length)
        self._sink = gr.message_sink(self._vblen, self._msgq, True)
        self._fg.connect(self._u, self._s2v, self._lpf, self._sink)

    def tune(self, frequency):
        if self._verbose:
            print "Setting receiver frequency to", n2s(frequency)
        result = self._u.tune(0, self._subdev, frequency)
        if result == False:
            raise RuntimeError("Failed to set receiver frequency.")

    def set_gain(self, gain):
        self._gain = gain
	if self._loopback:
	    return
	    
        if self._gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self._subdev.gain_range()
            self._gain = float(g[0]+g[1])/2
        if self._verbose:
            print "Setting receiver gain to", gain
        self._subdev.set_gain(self._gain)

    def start(self):
        if self._debug:
            print "Starting receiver flow graph."
        self._fg.start()

    def wait(self):
        if self._debug:
            print "Waiting for threads..."
        self._fg.wait()

    def stop(self):
        if self._debug:
            print "Stopping receiver flow graph."
        self._fg.stop()
        self.wait()
        if self._debug:
            print "Receiver flow graph stopped."


class sounder:
    def __init__(self,transmit=False,receive=False,loopback=False,rx_subdev_spec=None,ampl=0x1FFF,
                 frequency=0.0,rx_gain=None,degree=12,length=1,alpha=1.0,msgq=None,verbose=False,debug=False):
        self._transmit = transmit
        self._receive = receive
        self._loopback = loopback
        self._rx_subdev_spec = rx_subdev_spec
        self._frequency = frequency
        self._amplitude = ampl
        self._rx_gain = rx_gain
        self._degree = degree
        self._length = length
        self._alpha = alpha
        self._msgq = msgq
        self._verbose = verbose
        self._debug = debug
	self._mode = 0
	self._u = None
	self._trans = None
	self._rcvr = None
	self._transmitting = False
	self._receiving = False
		
	if self._transmit:
	    self._trans = sounder_tx(loopback=self._loopback,ampl=self._amplitude,
                                     verbose=self._verbose)
            self._u = self._trans._u
            
	if self._receive:
            self._rcvr = sounder_rx(subdev_spec=self._rx_subdev_spec,length=self._length,
                                    gain=self._rx_gain,alpha=self._alpha,msgq=self._msgq,
                                    loopback=self._loopback,verbose=self._verbose, 
				    debug=self._debug)
	    self._u = self._rcvr._u # either receiver or transmitter object will do
	
	self.set_reset(True)
        if self._loopback == False:
            self.tune(self._frequency)
	self.set_degree(self._degree)
	self.set_loopback(self._loopback)	
	self.set_reset(False)
		
    def tune(self, frequency):
        self._frequency = frequency
	if self._rcvr:
            self._rcvr.tune(frequency)
	if self._trans:
	    self._trans.tune(frequency)

    def set_degree(self, degree):
        if self._verbose:
            print "Setting PN code degree to", degree
        self._u._write_fpga_reg(FR_DEGREE, degree);
	    
    def _write_mode(self):
        if self._debug:
            print "Writing mode register with:", hex(self._mode)
        self._u._write_fpga_reg(FR_MODE, self._mode)

    def enable_tx(self, value):
	if value:
            if self._verbose:
                print "Enabling transmitter."
	    self._mode |= bmFR_MODE_TX
	    self._transmitting = True
	else:
            if self._verbose:
                print "Disabling transmitter."
	    self._mode &= ~bmFR_MODE_TX
	self._write_mode()
		    
    def enable_rx(self, value):
	if value:
	    self._mode |= bmFR_MODE_RX
	    self._write_mode()
	    self._rcvr.start()
	    self._receiving = True
	else:
            self._rcvr.stop()
	    self._mode &= ~bmFR_MODE_RX
	    self._write_mode()
	    self._receiving = False
	        
    def set_loopback(self, value):
	if value:
            if self._verbose:
                print "Enabling digital loopback."
	    self._mode |= bmFR_MODE_LP
	else:
            if self._verbose:
                print "Disabling digital loopback."
	    self._mode &= ~bmFR_MODE_LP
	self._write_mode()

    def set_reset(self, value):
	if value:
            if self._debug:
                print "Asserting reset."
	    self._mode |= bmFR_MODE_RESET
	else:
            if self._debug:
                print "De-asserting reset."
	    self._mode &= ~bmFR_MODE_RESET
	self._write_mode()

    def start(self):
        if self._transmit:
            self.enable_tx(True)
        if self._receive:
            self.enable_rx(True)

    def __del__(self):
	if self._transmitting:
	    self.enable_tx(False)
	    
	if self._receiving:
	    self.enable_rx(False)
	    
