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

from gnuradio import gr, usrp, eng_notation
n2s = eng_notation.num_to_str

# Hierarchical block implementing a USRP source for complex floats,
# with convenience functions for gain, tune, decimation, etc.
#
class usrp_source_c(gr.hier_block2):
    """
    Create a USRP source object supplying complex floats.
    """
    def __init__(self, which=0, subdev_spec=None, gain=None, if_rate=None,
                 freq=0.0, calibration=0.0, verbose=False):
	# Call hierarchical block constructor
	gr.hier_block2.__init__(self,
                                "usrp_source_c",                           # Block typename
				gr.io_signature(0,0,0),                    # Input signature
				gr.io_signature(1,1,gr.sizeof_gr_complex)) # Output signature

        self._verbose = verbose
        self._u = usrp.source_c(which)
        if self._verbose:
            print 'ADC sample rate is', n2s(self._u.adc_rate()), "sps"
        self.set_subdev(subdev_spec)
        self.set_if_rate(if_rate)
        self.set_gain(gain)
        self.set_calibration(calibration)
        self.tune(freq)
        self.connect(self._u, self)

    def set_subdev(self, subdev_spec):
        if subdev_spec is None:
            subdev_spec = self.pick_subdevice()
        self._subdev = usrp.selected_subdev(self._u, subdev_spec)
        self._u.set_mux(usrp.determine_rx_mux_value(self._u, subdev_spec))
        if self._verbose:
            print 'RX using', self._subdev.name(), 'daughterboard'
            
    def pick_subdevice(self):
        """
        The user didn't specify a subdevice.
        If there's a daughterboard on A, select A.
        If there's a daughterboard on B, select B.
        Otherwise, select A.
        """
        if self._u.db[0][0].dbid() >= 0:       # dbid is < 0 if there's no d'board or a problem
            return (0, 0)
        if self._u.db[1][0].dbid() >= 0:
            return (1, 0)
        return (0, 0)

    def set_if_rate(self, if_rate):
        # If no IF rate specified, set to maximum decimation
        if if_rate is None:
            self._decim = 256
        else:
            self._decim = int(self._u.adc_rate()/if_rate)

        self._u.set_decim_rate(self._decim)
        self._if_rate = self._u.adc_rate()/self._decim

        if self._verbose:
            print "USRP decimation rate is", self._decim
            print "USRP IF rate is", n2s(self._if_rate), "sps"
            
    def set_gain(self, gain):
	# If no gain specified, set to midrange
	if gain is None:
	    g = self._subdev.gain_range()
	    gain = (g[0]+g[1])/2.0
        self._gain = gain
        self._subdev.set_gain(self._gain)

    def set_calibration(self, calibration):
        self._cal = calibration
        if self._verbose:
            print "Using frequency calibration offset of", n2s(calibration), "Hz"
            
    def tune(self, freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @type: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        self._tune_result = usrp.tune(self._u, 0, self._subdev, freq+self._cal)
        if self._tune_result:
            if self._verbose:
                print "Baseband frequency is", n2s(self._tune_result.baseband_freq), "Hz"
                print "DXC frequency is", n2s(self._tune_result.dxc_freq), "Hz"
                print "Center frequency is", n2s(freq), "Hz"
                print "Residual frequency is", n2s(self._tune_result.residual_freq), "Hz"
            return True
        
        return False

if __name__ == '__main__':
    src = usrp_source_c(verbose=True)
