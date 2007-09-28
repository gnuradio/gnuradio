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
from gr import gr_threading as _threading

n2s = eng_notation.num_to_str

txp_delay = 60 # Pipeline delay to turn on transmit mixer
rxp_delay = 76 # Pipeline delay to turn off transmit mixer

#-----------------------------------------------------------------------
# FPGA Register Definitions
#-----------------------------------------------------------------------
FR_RADAR_MODE           = usrp.FR_USER_0    # Operational mode
bmFR_RADAR_MODE_RESET   = 1 << 0	# bit 0: active high reset
bmFR_RADAR_TXSIDE       = 1 << 1        # bit 1: use TX side A or B
#bmFR_RADAR_MODE_LP      = 1 << 2	# bit 2: enable digital loopback
#bmFR_RADAR_MODE_DR      = 1 << 3	# bit 3: enable on-board deramping
#bmFR_RADAR_MODE_MD      = 1 << 4   	# bit 4: enable echo metadata
#bmFR_RADAR_MODE_CHIRPS  = 3 << 5   	# bit 5,6: number of chirp center frequencies
bmFR_RADAR_DEBUG        = 1 << 7        # bit 7: enable debugging mode

FR_RADAR_TON    = usrp.FR_USER_1	# 16-bit transmitter on time in clocks
FR_RADAR_TSW    = usrp.FR_USER_2	# 16-bit transmitter switch time in clocks
FR_RADAR_TLOOK  = usrp.FR_USER_3	# 16-bit receiver look time in clocks
FR_RADAR_TIDLE  = usrp.FR_USER_4	# 32-bit inter-pulse idle time
FR_RADAR_AMPL   = usrp.FR_USER_5  	# 16-bit pulse amplitude (2s complement) into CORDIC
FR_RADAR_FSTART = usrp.FR_USER_6  	# 32-bit FTW for chirp start frequency
FR_RADAR_FINCR  = usrp.FR_USER_7  	# 32-bit FTW increment per transmit clock
FR_RADAR_ATRDEL = usrp.FR_USER_8        # 16-bit TX delay in clocks, 16-bit RX delay in clocks

# These are for phase II development (need to renumber)
#FR_RADAR_FREQ1N = usrp.FR_USER_8  # 24-bit N register for chirp #1
#FR_RADAR_FREQ1R = usrp.FR_USER_9  # 24-bit R register for chirp #1
#FR_RADAR_FREQ1C = usrp.FR_USER_10 # 24-bit C register for chirp #1
#FR_RADAR_FREQ2N = usrp.FR_USER_11 # 24-bit N register for chirp #2
#FR_RADAR_FREQ2R = usrp.FR_USER_12 # 24-bit R register for chirp #2
#FR_RADAR_FREQ2C = usrp.FR_USER_13 # 24-bit C register for chirp #2
#FR_RADAR_FREQ3N = usrp.FR_USER_14 # 24-bit N register for chirp #3
#FR_RADAR_FREQ3R = usrp.FR_USER_15 # 24-bit R register for chirp #3
#FR_RADAR_FREQ3C = usrp.FR_USER_16 # 24-bit C register for chirp #3
#FR_RADAR_FREQ4N = usrp.FR_USER_17 # 24-bit N register for chirp #4
#FR_RADAR_FREQ4R = usrp.FR_USER_18 # 24-bit R register for chirp #4
#FR_RADAR_FREQ4C = usrp.FR_USER_19 # 24-bit C register for chirp #4

#-----------------------------------------------------------------------
# Transmitter object.  Uses usrp_sink, but only for a handle to the
# FPGA registers.
#-----------------------------------------------------------------------
class radar_tx(object):
    def __init__(self, options):
        self._subdev_spec = options.tx_subdev_spec
	self._verbose = options.verbose
	self._debug = options.debug
        self._u = usrp.sink_s(fpga_filename='usrp_radar_mono.rbf')

        if self._subdev_spec == None:
            self._subdev_spec = usrp.pick_tx_subdevice(self._u)

        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)
	self._subdev.set_lo_offset(0.0)
	self._ton_ticks = 0
	self._tsw_ticks = 0
	self._tlook_ticks = 0
	self._tidle_ticks = 0
	
        if self._verbose:
            print "Using", self._subdev.name(), "for radar transmitter."            
	    
    def set_ton(self, ton):
	self._ton_ticks = 2*(int(ton*64e6)/2)-1  # Even number, then subtract 1
	if self._verbose:
	    print "Setting pulse on time to", ton, " sec ("+`self._ton_ticks+1`+" ticks)"
	self._u._write_fpga_reg(FR_RADAR_TON, self._ton_ticks)

    def set_tsw(self, tsw):
	self._tsw_ticks = 2*(int(tsw*64e6)/2)-1+rxp_delay  # Even number, then subtract 1
	if self._verbose:
	    print "Setting pulse switching time to", tsw, " sec ("+`self._tsw_ticks+1`+" ticks)"
	self._u._write_fpga_reg(FR_RADAR_TSW, self._tsw_ticks)

    def set_tlook(self, tlook):
	self._tlook_ticks = 2*(int(tlook*64e6)/2)-1  # Even number, then subtract 1
	if self._verbose:
	    print "Setting receiver look time to", tlook, " sec ("+`self._tlook_ticks+1`+" ticks)"
	self._u._write_fpga_reg(FR_RADAR_TLOOK, self._tlook_ticks)

    def set_prf(self, prf):
	period = 2*int(32e6/prf)
	self._tidle_ticks = period-(self._ton_ticks+self._tsw_ticks+self._tlook_ticks+3)-1
	if self._verbose:
	    print "Setting PRF to", prf, "Hz ("+`self._tidle_ticks+1`+" ticks idle time)"
	self._u._write_fpga_reg(FR_RADAR_TIDLE, self._tidle_ticks)

    def set_amplitude(self, ampl):
	self._amplitude = int(ampl*9946/100.0) # CORDIC gain correction
        self._u._write_fpga_reg(FR_RADAR_AMPL, self._amplitude)

    def set_freq(self, center_freq, chirp_width):
        self._center_freq = center_freq
        self._chirp_width = chirp_width
	self._fstart = -int((chirp_width/2)*(2**32)/32e6)
        self._fincr = int((chirp_width/16e6)*(2**32)/(self._ton_ticks+1))
                          
        if self._verbose:
            print "Setting transmitter center frequency to", n2s(center_freq)
            print "Setting chirp width to", n2s(chirp_width), "Hz "+"("+hex(self._fstart)+", "+hex(self._fincr)+")"

        result = self._u.tune(0, self._subdev, center_freq)
        if result == False:
            raise RuntimeError("Failed to set transmitter frequency.")
	self._u._write_fpga_reg(FR_RADAR_FSTART, self._fstart)
        self._u._write_fpga_reg(FR_RADAR_FINCR, self._fincr)

    def start(self):
        self._u.start()
	self._subdev.set_enable(True)
	
    def stop(self):
	self._subdev.set_enable(False)
	self._u.stop()

    def subdev_spec(self):
        return self._subdev_spec
    
    def echo_length(self):
        return self._tlook_ticks+1

    def __del__(self):
	del self._subdev # Avoid weak reference error
	
#-----------------------------------------------------------------------
# Receiver object.  Uses usrp_source_c to receive echo records.
#-----------------------------------------------------------------------
class radar_rx(gr.top_block):
    def __init__(self, options, callback):
	gr.top_block.__init__(self, "radar_rx")

	self._u = None
        self._subdev_spec = options.rx_subdev_spec
        self._gain = options.gain
	self._verbose = options.verbose
        self._debug = options.debug
        self._callback = callback
	self._length_set = False
	self._connected = False
	self._frequency = 0.0
        self._msgq = gr.msg_queue()
        self._watcher = _queue_watcher_thread(self._msgq, self._callback)
	        
    def set_echo_length(self, length):
        # Only call once
	if self._length_set is True:
	    raise RuntimeError("Can only set echo length once.")
	self._length = length
	self._length_set = True

    def tune(self, frequency):
        if self._verbose:
            print "Setting receiver frequency to", n2s(frequency)
	self._frequency = frequency
	if (self._u):
            result = self._u.tune(0, self._subdev, frequency)
	    if result == False:
    	        raise RuntimeError("Failed to set receiver frequency.")

    def set_gain(self, gain):
        self._gain = gain
        if self._gain == None:
            # if no gain was specified, use the mid-point in dB
            g = self._subdev.gain_range()
            self._gain = float(g[0]+g[1])/2
        self._subdev.set_gain(self._gain)

    def begin(self):
	if not self._connected:
	    self._setup_connections()
	    
        if self._verbose:
            print "Starting receiver..."
        self.start()

    def end(self):
        if self._verbose:
            print "Stopping receiver..."
        self.stop()
	self.wait()
        if self._verbose:
            print "Receiver stopped."

    def _setup_usrp(self):
        self._u = usrp.source_c(fpga_filename='usrp_radar_mono.rbf')
        if self._subdev_spec == None:
            self._subdev_spec = usrp.pick_rx_subdevice(self._u)
        self._u.set_mux(usrp.determine_rx_mux_value(self._u, self._subdev_spec))
        self._subdev = usrp.selected_subdev(self._u, self._subdev_spec)

        if self._verbose:
            print "Using", self._subdev.side_and_name(), "for radar receiver."
            print "Setting receiver gain to", self._gain
        self.set_gain(self._gain)
	self.tune(self._frequency)
			
    def _setup_connections(self):
	if not self._length_set:
	    raise RuntimeError("Echo length not set.")
	self._setup_usrp()
        self._vblen = gr.sizeof_gr_complex*self._length
	self._s2v = gr.stream_to_vector(gr.sizeof_gr_complex, self._length)
        self._sink = gr.message_sink(self._vblen, self._msgq, False)
        self.connect(self._u, self._s2v, self._sink)

        if self._verbose:
            print "Generating echo vectors of length", self._length, \
                  "(samples)", self._vblen, "(bytes)"

	self._connected = True

class radar(object):
    def __init__(self, options, callback):
	 
        self._verbose = options.verbose
        self._debug = options.debug

	self._mode = 0
        self._trans = radar_tx(options)	
        self._rcvr = radar_rx(options, callback)
	self.set_reset(True)
	self.set_tx_board(self._trans.subdev_spec())
        self.set_debug(self._debug)
	self.set_atrdel()
	        
    def _write_mode(self):
        self._trans._u._write_fpga_reg(FR_RADAR_MODE, self._mode)

    def set_reset(self, value):
	if value:
	    self._mode |= bmFR_RADAR_MODE_RESET
	else:
	    self._mode &= ~bmFR_RADAR_MODE_RESET
	self._write_mode()

    def set_tx_board(self, tx_subdev_spec):
	if tx_subdev_spec[0] == 1:
	    self._mode |= bmFR_RADAR_TXSIDE
	else:
	    self._mode &= ~bmFR_RADAR_TXSIDE
	self._write_mode()
	
    def set_debug(self, value):
	if value:
	    self._mode |= bmFR_RADAR_DEBUG
	else:
	    self._mode &= ~bmFR_RADAR_DEBUG
	self._write_mode()
    
    def set_ton(self, ton):
	self._trans.set_ton(ton)
	
    def set_tsw(self, tsw):
	self._trans.set_tsw(tsw)
	
    def set_tlook(self, tlook):
	self._trans.set_tlook(tlook)
	self._rcvr.set_echo_length(self._trans.echo_length())
	
    def set_prf(self, prf):
	self._trans.set_prf(prf)

    def set_amplitude(self, ampl):
        self._trans.set_amplitude(ampl)

    def set_freq(self, center_freq, chirp_width):
        self._trans.set_freq(center_freq, chirp_width)
	self._rcvr.tune(center_freq)
	        
    def set_atrdel(self, tx_delay=txp_delay, rx_delay=rxp_delay):
        if self._verbose:
	    print "Setting TX delay of", tx_delay, "clocks, RX delay of", rx_delay
        self._trans._u._write_fpga_reg(FR_RADAR_ATRDEL, tx_delay << 16 | rx_delay)

    def start(self):
	self.set_reset(False)
	self._trans.start()
	self._rcvr.begin()
	
    def stop(self):
	self._rcvr.end()
	self._trans.stop()
	self.set_reset(True)

#-----------------------------------------------------------------------
# Queue watcher.  Dispatches received echos to callback.
#-----------------------------------------------------------------------
class _queue_watcher_thread(_threading.Thread):
    def __init__(self, msgq, callback):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self._msgq = msgq
        self._callback = callback
        self._keep_running = True
        self.start()

    def run(self):
        while self._keep_running == True:
            msg = self._msgq.delete_head()
            if self._callback:
                self._callback(msg.to_string())

