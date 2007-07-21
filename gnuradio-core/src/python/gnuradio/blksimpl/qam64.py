#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

"""
differential QPSK modulation and demodulation.
"""

from gnuradio import gr, gru, modulation_utils
from math import pi, sqrt
import qam
import cmath
from pprint import pprint

# default values (used in __init__ and add_options)
_def_samples_per_symbol = 2
_def_excess_bw = 0.35
_def_gray_code = True
_def_verbose = False
_def_log = False

_def_costas_alpha = None
_def_gain_mu = 0.03
_def_mu = 0.05
_def_omega_relative_limit = 0.005


# /////////////////////////////////////////////////////////////////////////////
#                           QAM64 modulator
# /////////////////////////////////////////////////////////////////////////////

class qam64_mod(gr.hier_block):

    def __init__(self, fg,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 gray_code=_def_gray_code,
                 verbose=_def_verbose,
                 log=_def_log):

        """
	Hierarchical block for RRC-filtered QPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

	@param fg: flow graph
	@type fg: flow graph
	@param samples_per_symbol: samples per symbol >= 2
	@type samples_per_symbol: integer
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param gray_code: Tell modulator to Gray code the bits
        @type gray_code: bool
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param debug: Print modualtion data to files?
        @type debug: bool
	"""

        self._fg = fg
        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._gray_code = gray_code

        if not isinstance(samples_per_symbol, int) or samples_per_symbol < 2:
            raise TypeError, ("sbp must be an integer >= 2, is %d" % samples_per_symbol)

	ntaps = 11 * samples_per_symbol
 
        arity = pow(2, self.bits_per_symbol())

        # turn bytes into k-bit vectors
        self.bytes2chunks = \
          gr.packed_to_unpacked_bb(self.bits_per_symbol(), gr.GR_MSB_FIRST)

        if self._gray_code:
            self.symbol_mapper = gr.map_bb(qam.binary_to_gray[arity])
        else:
            self.symbol_mapper = gr.map_bb(qam.binary_to_ungray[arity])
            
        self.diffenc = gr.diff_encoder_bb(arity)

        rot = 1.0
        print "constellation with %d arity" % arity
        rotated_const = map(lambda pt: pt * rot, qam.constellation[arity])
        self.chunks2symbols = gr.chunks_to_symbols_bc(rotated_const)

        # pulse shaping filter
	self.rrc_taps = gr.firdes.root_raised_cosine(
	    self._samples_per_symbol, # gain  (sps since we're interpolating by sps)
            self._samples_per_symbol, # sampling rate
            1.0,		      # symbol rate
            self._excess_bw,          # excess bandwidth (roll-off factor)
            ntaps)

	self.rrc_filter = gr.interp_fir_filter_ccf(self._samples_per_symbol, self.rrc_taps)

        if verbose:
            self._print_verbage()
        
        if log:
            self._setup_logging()
            
	# Connect & Initialize base class
        self._fg.connect(self.bytes2chunks, self.symbol_mapper, self.diffenc,
                         self.chunks2symbols, self.rrc_filter)
	gr.hier_block.__init__(self, self._fg, self.bytes2chunks, self.rrc_filter)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 6
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.  RTFM

    def _print_verbage(self):
        print "bits per symbol = %d" % self.bits_per_symbol()
        print "Gray code = %s" % self._gray_code
        print "RRS roll-off factor = %f" % self._excess_bw

    def _setup_logging(self):
        print "Modulation logging turned on."
        self._fg.connect(self.bytes2chunks,
                         gr.file_sink(gr.sizeof_char, "bytes2chunks.dat"))
        self._fg.connect(self.symbol_mapper,
                         gr.file_sink(gr.sizeof_char, "graycoder.dat"))
        self._fg.connect(self.diffenc,
                         gr.file_sink(gr.sizeof_char, "diffenc.dat"))        
        self._fg.connect(self.chunks2symbols,
                         gr.file_sink(gr.sizeof_gr_complex, "chunks2symbols.dat"))
        self._fg.connect(self.rrc_filter,
                         gr.file_sink(gr.sizeof_gr_complex, "rrc_filter.dat"))

    def add_options(parser):
        """
        Adds QAM modulation-specific options to the standard parser
        """
        parser.add_option("", "--excess-bw", type="float", default=_def_excess_bw,
                          help="set RRC excess bandwith factor [default=%default] (PSK)")
        parser.add_option("", "--no-gray-code", dest="gray_code",
                          action="store_false", default=_def_gray_code,
                          help="disable gray coding on modulated bits (PSK)")
    add_options=staticmethod(add_options)


    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(qam64_mod.__init__,
                                                            ('self', 'fg'), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)


# /////////////////////////////////////////////////////////////////////////////
#                           QAM16 demodulator
#
# /////////////////////////////////////////////////////////////////////////////

class qam64_demod(gr.hier_block):

    def __init__(self, fg,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 costas_alpha=_def_costas_alpha,
                 gain_mu=_def_gain_mu,
                 mu=_def_mu,
                 omega_relative_limit=_def_omega_relative_limit,
                 gray_code=_def_gray_code,
                 verbose=_def_verbose,
                 log=_def_log):

        # do this
        pass
    
    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 6
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.  RTFM

#
# Add these to the mod/demod registry
#
# NOT READY TO BE USED YET -- ENABLE AT YOUR OWN RISK
#modulation_utils.add_type_1_mod('qam64', qam64_mod)
#modulation_utils.add_type_1_demod('qam16', qam16_demod)
