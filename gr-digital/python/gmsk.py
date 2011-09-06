#
# GMSK modulation and demodulation.  
#
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

# See gnuradio-examples/python/digital for examples

from gnuradio import gr
import digital_swig
import modulation_utils2

from math import pi
import numpy
from pprint import pprint
import inspect

# default values (used in __init__ and add_options)
_def_samples_per_symbol = 2
_def_bt = 0.35
_def_verbose = False
_def_log = False

# Symbol timing recovery 
_def_timing_bw = 2*pi/100.0
_def_timing_max_dev = 1.5

# /////////////////////////////////////////////////////////////////////////////
#                              GMSK modulator
# /////////////////////////////////////////////////////////////////////////////

class gmsk_mod(gr.hier_block2):

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 bt=_def_bt,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK)
	modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

	@param samples_per_symbol: samples per baud >= 2
	@type samples_per_symbol: integer
	@param bt: Gaussian filter bandwidth * symbol time
	@type bt: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param debug: Print modualtion data to files?
        @type debug: bool       
	"""

	gr.hier_block2.__init__(self, "gmsk_mod",
				gr.io_signature(1, 1, gr.sizeof_char),       # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._bt = bt

        if not isinstance(samples_per_symbol, int) or samples_per_symbol < 2:
            raise TypeError, ("samples_per_symbol must be an integer >= 2, is %r" % \
                                  (samples_per_symbol,))

	ntaps = 4 * samples_per_symbol			# up to 3 bits in filter at once
	sensitivity = (pi / 2) / samples_per_symbol	# phase change per bit = pi / 2

	# Turn it into NRZ data.
	self.nrz = gr.bytes_to_syms()

	# Form Gaussian filter
        # Generate Gaussian response (Needs to be convolved with window below).
	self.gaussian_taps = gr.firdes.gaussian(
		1,		       # gain
		samples_per_symbol,    # symbol_rate
		bt,		       # bandwidth * symbol time
		ntaps	               # number of taps
		)

	self.sqwave = (1,) * samples_per_symbol       # rectangular window
	self.taps = numpy.convolve(numpy.array(self.gaussian_taps),numpy.array(self.sqwave))
	self.gaussian_filter = gr.interp_fir_filter_fff(samples_per_symbol, self.taps)

	# FM modulation
	self.fmmod = gr.frequency_modulator_fc(sensitivity)
		
        if verbose:
            self._print_verbage()
         
        if log:
            self._setup_logging()

	# Connect & Initialize base class
	self.connect(self, self.nrz, self.gaussian_filter, self.fmmod, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):     # staticmethod that's also callable on an instance
        return 1
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.


    def _print_verbage(self):
        print "bits per symbol = %d" % self.bits_per_symbol()
        print "Gaussian filter bt = %.2f" % self._bt


    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.nrz,
                     gr.file_sink(gr.sizeof_float, "tx_gmsk_nrz.32f"))
        self.connect(self.gaussian_filter,
                     gr.file_sink(gr.sizeof_float, "tx_gmsk_gaussian_filter.32f"))
        self.connect(self.fmmod,
                     gr.file_sink(gr.sizeof_gr_complex, "tx_gmsk_fmmod.32fc"))


    def add_options(parser):
        """
        Adds GMSK modulation-specific options to the standard parser
        """
        parser.add_option("", "--bt", type="float", default=_def_bt,
                          help="set bandwidth-time product [default=%default] (GMSK)")
    add_options=staticmethod(add_options)


    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils2.extract_kwargs_from_options(gmsk_mod.__init__,
                                                            ('self',), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)



# /////////////////////////////////////////////////////////////////////////////
#                            GMSK demodulator
# /////////////////////////////////////////////////////////////////////////////

class gmsk_demod(gr.hier_block2):

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 bt=_def_bt,
                 timing_bw=_def_timing_bw,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for Gaussian Minimum Shift Key (GMSK)
	demodulation.

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (the LSB)

	@param samples_per_symbol: samples per baud
	@type samples_per_symbol: integer
        @param timing_bw: timing recovery loop lock-in bandwidth
        @type timing_bw: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Print modualtion data to files?
        @type log: bool 
	"""

	gr.hier_block2.__init__(self, "gmsk_demod",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_char))       # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._bt = bt
        self._timing_bw = timing_bw
        self._timing_max_dev= _def_timing_max_dev
        
        if samples_per_symbol < 2:
            raise TypeError, "samples_per_symbol >= 2, is %f" % samples_per_symbol

	# Demodulate FM
	sensitivity = (pi / 2) / samples_per_symbol
	self.fmdemod = gr.quadrature_demod_cf(1.0 / sensitivity)

	# the clock recovery block tracks the symbol clock and resamples as needed.
	# the output of the block is a stream of soft symbols (float)
        nfilts = 32
        ntaps = 11 * int(self._samples_per_symbol*nfilts) 
	taps = gr.firdes.gaussian(nfilts,
                                  nfilts*self._samples_per_symbol,
                                  self._bt, ntaps)
        self.clock_recovery = \
            gr.pfb_clock_sync_fff(self._samples_per_symbol,
                                  self._timing_bw, taps,
                                  nfilts, nfilts//2, self._timing_max_dev)
            
        # slice the floats at 0, outputting 1 bit (the LSB of the output byte) per sample
        self.slicer = digital_swig.binary_slicer_fb()

        if verbose:
            self._print_verbage()
         
        if log:
            self._setup_logging()

	# Connect & Initialize base class
	self.connect(self, self.fmdemod, self.clock_recovery, self.slicer, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 1
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.


    def _print_verbage(self):
        print "bits per symbol:     %d" % self.bits_per_symbol()
        print "Bandwidth-Time Prod: %f" % self._bw
        print "Timing bandwidth:    %.2e" % self._timing_bw


    def _setup_logging(self):
        print "Demodulation logging turned on."
        self.connect(self.fmdemod,
                    gr.file_sink(gr.sizeof_float, "rx_gmsk_fmdemod.32f"))
        self.connect(self.clock_recovery,
                    gr.file_sink(gr.sizeof_float, "rx_gmsk_clock_recovery.32f"))
        self.connect(self.slicer,
                    gr.file_sink(gr.sizeof_char, "rx_gmsk_slicer.8b"))

    def add_options(parser):
        """
        Adds GMSK demodulation-specific options to the standard parser
        """
        parser.add_option("", "--timing-bw", type="float", default=_def_timing_bw,
                          help="set timing symbol sync loop gain lock-in bandwidth [default=%default]")
    add_options=staticmethod(add_options)

    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils2.extract_kwargs_from_options(gmsk_demod.__init__,
                                                             ('self',), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)

#
# Add these to the mod/demod registry
#
modulation_utils2.add_type_1_mod('gmsk', gmsk_mod)
modulation_utils2.add_type_1_demod('gmsk', gmsk_demod)
