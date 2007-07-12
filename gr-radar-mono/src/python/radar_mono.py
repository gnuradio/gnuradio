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

n2s = eng_notation.num_to_str

#-----------------------------------------------------------------------
# FPGA Register Definitions
#-----------------------------------------------------------------------
FR_RADAR_MODE           = usrp.FR_USER_0    # Operational mode
bmFR_RADAR_MODE_RESET   = 1 << 0	# bit 0: active high reset
bmFR_RADAR_MODE_TX      = 1 << 1	# bit 1: enable transmitter
#bmFR_RADAR_MODE_RX     = 1 << 2	# bit 2: enable receiver
#bmFR_RADAR_MODE_LP     = 1 << 3	# bit 3: enable digital loopback
#bmFR_RADAR_MODE_DR     = 1 << 4	# bit 4: enable on-board deramping
#bmFR_RADAR_MODE_MD     = 1 << 5   	# bit 5: enable echo metadata
#bmFR_RADAR_MODE_CHIRPS = 3 << 6   	# bit 6,7: number of chirp center frequencies

#FR_RADAR_TON    = usrp.FR_USER_1	# 16-bit transmitter on time in clocks
#FR_RADAR_TSW    = usrp.FR_USER_2	# 16-bit transmitter switch time in clocks
#FR_RADAR_TLOOK  = usrp.FR_USER_3	# 16-bit receiver look time in clocks
#FR_RADAR_TIDLE  = usrp.FR_USER_4	# 32-bit inter-pulse idle time
FR_RADAR_AMPL   = usrp.FR_USER_5  	# 16-bit pulse amplitude (2s complement) into CORDIC
#FR_RADAR_FSTART = usrp.FR_USER_6  	# 32-bit FTW for chirp start frequency
#FR_RADAR_FINCR  = usrp.FR_USER_7  	# 32-bit FTW increment per transmit clock

# These are for phase II development

# Temporarily use this for transmitter frequency calibration
FR_RADAR_FREQ1N = usrp.FR_USER_8  # 24-bit N register for chirp #1

FR_RADAR_FREQ1R = usrp.FR_USER_9  # 24-bit R register for chirp #1
FR_RADAR_FREQ1C = usrp.FR_USER_10 # 24-bit control register for chirp #1
FR_RADAR_FREQ2N = usrp.FR_USER_11 # 24-bit N register for chirp #2
FR_RADAR_FREQ2R = usrp.FR_USER_12 # 24-bit R register for chirp #2
FR_RADAR_FREQ2C = usrp.FR_USER_13 # 24-bit control register for chirp #2
FR_RADAR_FREQ3N = usrp.FR_USER_14 # 24-bit N register for chirp #3
FR_RADAR_FREQ3R = usrp.FR_USER_15 # 24-bit R register for chirp #3
FR_RADAR_FREQ3C = usrp.FR_USER_16 # 24-bit control register for chirp #3
FR_RADAR_FREQ4N = usrp.FR_USER_17 # 24-bit N register for chirp #4
FR_RADAR_FREQ4R = usrp.FR_USER_18 # 24-bit R register for chirp #4
FR_RADAR_FREQ4C = usrp.FR_USER_19 # 24-bit control register for chirp #4

#-----------------------------------------------------------------------
# Transmitter object.  Uses usrp_sink, but only for a handle to the
# FPGA registers.
#-----------------------------------------------------------------------
class radar_tx:
    def __init__(self, verbose=False, debug=False):
	self._verbose = verbose
	self._debug = debug

        self._u = usrp.sink_s(fpga_filename='usrp_radar_mono.rbf')
        self._subdev_spec = (0,0); # FPGA code only implements side A
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
        if self._verbose:
            print "Using", self._subdev.name(), "for radar transmitter."            

    def tune(self, center_freq, waveform_freq):
        self._center_freq = center_freq
        self._waveform_freq = waveform_freq
	self._ftw = int(waveform_freq*(2**32)/32e6)
        if self._verbose:
            print "Setting transmitter center frequency to", n2s(center_freq)
            print "Setting waveform frequency offset to", n2s(waveform_freq), "with ftw of", self._ftw
        result = self._u.tune(0, self._subdev, center_freq)
        if result == False:
            raise RuntimeError("Failed to set transmitter frequency.")
	self._u._write_fpga_reg(FR_RADAR_FREQ1N, self._ftw)

    def set_amplitude(self, ampl):
	self._amplitude = int(ampl*9946/100.0) # CORDIC gain correction
        if self._debug:
            print "Writing amplitude register with:", hex(self._amplitude)
        self._u._write_fpga_reg(FR_RADAR_AMPL, self._amplitude)

    def start(self):
        self._u.start()

    def stop(self):
	self._u.stop()
	
#-----------------------------------------------------------------------
# Receiver object.  Uses usrp_source_c to receive echo records.
# NOT IMPLEMENTED YET
#-----------------------------------------------------------------------
"""
class radar_rx:
    def __init__(self,gain=None,msgq=None,loopback=False,verbose=False,debug=False):
        self._gain = gain
        self._msgq = msgq
	self._loopback = loopback
	self._verbose = verbose
	self._debug = debug
		
        self._fg = gr.flow_graph()
        self._u = usrp.source_c(fpga_filename='usrp_radar.rbf')
	if not self._loopback:
            self._subdev_spec = (0,0) # FPGA only implements side A
            self._u.set_mux(usrp.determine_rx_mux_value(self._u, self._subdev_spec))
            self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
            if self._verbose:
	        print "Using", self._subdev.name(), "for radar receiver."

        self.set_gain(self._gain)

        # need to compute length here
        
        self._vblen = gr.sizeof_gr_complex*self._length
        if self._debug:
            print "Generating echo vectors of length", self._length, "byte length", self._vblen
            
        self._s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self._length)
        self._sink = gr.message_sink(self._vblen, self._msgq, True)
        self._fg.connect(self._u, self._s2v, self._sink)
        
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
"""

class radar:
    def __init__(self,msgq=None,verbose=False,debug=False):
        self._msgq = msgq
        self._verbose = verbose
        self._debug = debug

	self._mode = 0
	self._transmitting = False
        self._trans = radar_tx(verbose=self._verbose, debug=self._debug)
	self.set_reset(True)
		
    def set_amplitude(self, ampl):
        self._trans.set_amplitude(ampl)

    def tune(self, center_freq, waveform_freq):
        self._trans.tune(center_freq, waveform_freq)

    def _write_mode(self):
        if self._debug:
            print "Writing mode register with:", hex(self._mode)
        self._trans._u._write_fpga_reg(FR_RADAR_MODE, self._mode)

    def enable_tx(self, value):
	if value:
            if self._verbose:
                print "Enabling transmitter."
	    self._mode |= bmFR_RADAR_MODE_TX
	    self._transmitting = True
	else:
            if self._verbose:
                print "Disabling transmitter."
	    self._mode &= ~bmFR_RADAR_MODE_TX
	    self._transmitting = False
	self._write_mode()
		    
    """
    def enable_rx(self, value):
	if value:
	    self._mode |= bmFR_RADAR_MODE_RX
	    self._write_mode()
	    self._rcvr.start()
	    self._receiving = True
	else:
            self._rcvr.stop()
	    self._mode &= ~bmFR_RADAR_MODE_RX
	    self._write_mode()
	    self._receiving = False
    """
    """
    def set_loopback(self, value):
	if value:
            if self._verbose:
                print "Enabling digital loopback."
	    self._mode |= bmFR_RADAR_MODE_LP
	else:
            if self._verbose:
                print "Disabling digital loopback."
	    self._mode &= ~bmFR_RADAR_MODE_LP
	self._write_mode()
    """
    
    def set_reset(self, value):
	if value:
            if self._debug:
                print "Asserting reset."
	    self._mode |= bmFR_RADAR_MODE_RESET
	else:
            if self._debug:
                print "De-asserting reset."
	    self._mode &= ~bmFR_RADAR_MODE_RESET
	self._write_mode()

    def start(self):
	self._trans.start()
	self.enable_tx(True)
	self.set_reset(False)
	
    def stop(self):
	self.set_reset(True)
	self._trans.stop()
	if self._transmitting:
	    self.enable_tx(False)

    def __del__(self):
        self.stop()
