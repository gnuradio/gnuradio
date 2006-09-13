#
# GMSK modulation and demodulation.  
#
#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

# See gnuradio-examples/python/gmsk2 for examples

from gnuradio import gr
from math import pi
import Numeric

# /////////////////////////////////////////////////////////////////////////////
#            GMSK mod/demod with steams of bytes as data i/o
# /////////////////////////////////////////////////////////////////////////////

class gmsk2_mod(gr.hier_block):

    def __init__(self, fg, spb = 2, bt = 0.3):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK)
	modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

	@param fg: flow graph
	@type fg: flow graph
	@param spb: samples per baud >= 2
	@type spb: integer
	@param bt: Gaussian filter bandwidth * symbol time
	@type bt: float
	"""
        if not isinstance(spb, int) or spb < 2:
            raise TypeError, "sbp must be an integer >= 2"
        self.spb = spb

	ntaps = 4 * spb			# up to 3 bits in filter at once
	sensitivity = (pi / 2) / spb	# phase change per bit = pi / 2

	# Turn it into NRZ data.
	self.nrz = gr.bytes_to_syms()

	# Form Gaussian filter

        # Generate Gaussian response (Needs to be convolved with window below).
	self.gaussian_taps = gr.firdes.gaussian(
		1,		# gain
		spb,            # symbol_rate
		bt,		# bandwidth * symbol time
		ntaps	        # number of taps
		)

	self.sqwave = (1,) * spb       # rectangular window
	self.taps = Numeric.convolve(Numeric.array(self.gaussian_taps),Numeric.array(self.sqwave))
	self.gaussian_filter = gr.interp_fir_filter_fff(spb, self.taps)

	# FM modulation
	self.fmmod = gr.frequency_modulator_fc(sensitivity)
		
	# Connect
	fg.connect(self.nrz, self.gaussian_filter, self.fmmod)

	# Initialize base class
	gr.hier_block.__init__(self, fg, self.nrz, self.fmmod)

    def samples_per_baud(self):
        return self.spb

    def bits_per_baud(self=None):   # staticmethod that's also callable on an instance
        return 1
    bits_per_baud = staticmethod(bits_per_baud)      # make it a static method.  RTFM


class gmsk2_demod(gr.hier_block):

    def __init__(self, fg, spb=2, omega=None, gain_mu=0.03, mu=0.5,
                 omega_relative_limit=0.000200, freq_error=0.0):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK)
	demodulation.

	The input is the complex modulated signal at baseband.
	The output is a stream of symbols ready to be sliced at zero.

	@param fg: flow graph
	@type fg: flow graph
	@param spb: samples per baud
	@type spb: integer

        Clock recovery parameters.  These all have reasonble defaults.
        
        @param omega: nominal relative freq (defaults to spb)
        @type omega: float
        @param gain_mu: controls rate of mu adjustment
        @type gain_mu: float
        @param mu: fractional delay [0.0, 1.0]
        @type mu: float
        @param omega_relative_limit: sets max variation in omega
        @type omega_relative_limit: float, typically 0.000200 (200 ppm)
        @param freq_error: bit rate error as a fraction
        @param float
	"""
        if spb < 2:
            raise TypeError, "sbp >= 2"
        self.spb = spb
        
        if omega is None:
            omega = spb*(1+freq_error)

	gain_omega = .25*gain_mu*gain_mu        # critically damped

        # Automatic gain control
        self.preamp = gr.multiply_const_cc(10e-5)
        self.agc = gr.agc_cc(1e-3, 1, 1, 1000)

	# Demodulate FM
	sensitivity = (pi / 2) / spb
	self.fmdemod = gr.quadrature_demod_cf(1.0 / sensitivity)

        alpha = 0.0008

	# the clock recovery block tracks the symbol clock and resamples as needed.
	# the output of the block is a stream of soft symbols (float)
	self.clock_recovery = gr.clock_recovery_mm_ff(omega, gain_omega, mu, gain_mu,
                                                      omega_relative_limit)

        # slice the floats at 0, outputting 1 bit (the LSB of the output byte) per sample
        self.slicer = gr.binary_slicer_fb()

	fg.connect(self.preamp, self.agc, self.fmdemod, self.clock_recovery, self.slicer)
        
	# Initialize base class
	gr.hier_block.__init__(self, fg, self.preamp, self.slicer)

    def samples_per_baud(self):
        return self.spb

    def bits_per_baud(self=None):   # staticmethod that's also callable on an instance
        return 1
    bits_per_baud = staticmethod(bits_per_baud)      # make it a static method.  RTFM
