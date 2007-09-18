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

"""
differential BPSK modulation and demodulation.
"""

from gnuradio import gr, gru, modulation_utils
from math import pi, sqrt
import psk
import cmath
from pprint import pprint

# default values (used in __init__ and add_options)
_def_samples_per_symbol = 2
_def_excess_bw = 0.35
_def_gray_code = True
_def_verbose = False
_def_log = False

_def_costas_alpha = 0.1
_def_gain_mu = None
_def_mu = 0.5
_def_omega_relative_limit = 0.005


# /////////////////////////////////////////////////////////////////////////////
#                             DBPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class dbpsk_mod(gr.hier_block2):

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 gray_code=_def_gray_code,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for RRC-filtered differential BPSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.
        
	@param samples_per_symbol: samples per baud >= 2
	@type samples_per_symbol: integer
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param gray_code: Tell modulator to Gray code the bits
        @type gray_code: bool
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Log modulation data to files?
        @type log: bool
	"""

	gr.hier_block2.__init__(self, "dbpsk_mod",
				gr.io_signature(1, 1, gr.sizeof_char),       # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._gray_code = gray_code

        if not isinstance(self._samples_per_symbol, int) or self._samples_per_symbol < 2:
            raise TypeError, ("sbp must be an integer >= 2, is %d" % self._samples_per_symbol)
        
	ntaps = 11 * self._samples_per_symbol

        arity = pow(2,self.bits_per_symbol())
        
        # turn bytes into k-bit vectors
        self.bytes2chunks = \
          gr.packed_to_unpacked_bb(self.bits_per_symbol(), gr.GR_MSB_FIRST)

        if self._gray_code:
            self.symbol_mapper = gr.map_bb(psk.binary_to_gray[arity])
        else:
            self.symbol_mapper = gr.map_bb(psk.binary_to_ungray[arity])

        self.diffenc = gr.diff_encoder_bb(arity)

        self.chunks2symbols = gr.chunks_to_symbols_bc(psk.constellation[arity])

        # pulse shaping filter
	self.rrc_taps = gr.firdes.root_raised_cosine(
	    self._samples_per_symbol,   # gain (samples_per_symbol since we're
                                        # interpolating by samples_per_symbol)
	    self._samples_per_symbol,   # sampling rate
	    1.0,		        # symbol rate
	    self._excess_bw,            # excess bandwidth (roll-off factor)
            ntaps)
	self.rrc_filter = gr.interp_fir_filter_ccf(self._samples_per_symbol,
                                                   self.rrc_taps)

	# Connect
        self.connect(self, self.bytes2chunks, self.symbol_mapper, self.diffenc,
                     self.chunks2symbols, self.rrc_filter, self)

        if verbose:
            self._print_verbage()
            
        if log:
            self._setup_logging()
            

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):   # static method that's also callable on an instance
        return 1
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.  RTFM

    def add_options(parser):
        """
        Adds DBPSK modulation-specific options to the standard parser
        """
        parser.add_option("", "--excess-bw", type="float", default=_def_excess_bw,
                          help="set RRC excess bandwith factor [default=%default]")
        parser.add_option("", "--no-gray-code", dest="gray_code",
                          action="store_false", default=True,
                          help="disable gray coding on modulated bits (PSK)")
    add_options=staticmethod(add_options)

    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(dbpsk_mod.__init__,
                                                            ('self',), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)


    def _print_verbage(self):
        print "\nModulator:"
        print "bits per symbol:     %d" % self.bits_per_symbol()
        print "Gray code:           %s" % self._gray_code
        print "RRC roll-off factor: %.2f" % self._excess_bw

    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.bytes2chunks,
                     gr.file_sink(gr.sizeof_char, "tx_bytes2chunks.dat"))
        self.connect(self.symbol_mapper,
                     gr.file_sink(gr.sizeof_char, "tx_graycoder.dat"))
        self.connect(self.diffenc,
                     gr.file_sink(gr.sizeof_char, "tx_diffenc.dat"))
        self.connect(self.chunks2symbols,
                     gr.file_sink(gr.sizeof_gr_complex, "tx_chunks2symbols.dat"))
        self.connect(self.rrc_filter,
                     gr.file_sink(gr.sizeof_gr_complex, "tx_rrc_filter.dat"))
              

# /////////////////////////////////////////////////////////////////////////////
#                             DBPSK demodulator
#
#      Differentially coherent detection of differentially encoded BPSK
# /////////////////////////////////////////////////////////////////////////////

class dbpsk_demod(gr.hier_block2):

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 costas_alpha=_def_costas_alpha,
                 gain_mu=_def_gain_mu,
                 mu=_def_mu,
                 omega_relative_limit=_def_omega_relative_limit,
                 gray_code=_def_gray_code,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for RRC-filtered differential BPSK demodulation

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (LSB)

	@param samples_per_symbol: samples per symbol >= 2
	@type samples_per_symbol: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param costas_alpha: loop filter gain
        @type costas_alphas: float
        @param gain_mu: for M&M block
        @type gain_mu: float
        @param mu: for M&M block
        @type mu: float
        @param omega_relative_limit: for M&M block
        @type omega_relative_limit: float
        @param gray_code: Tell modulator to Gray code the bits
        @type gray_code: bool
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param debug: Print modualtion data to files?
        @type debug: bool
	"""
        
	gr.hier_block2.__init__(self, "dbpsk_demod",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_char))       # Output signature
				
        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._costas_alpha = costas_alpha
        self._mm_gain_mu = gain_mu
        self._mm_mu = mu
        self._mm_omega_relative_limit = omega_relative_limit
        self._gray_code = gray_code
        
        if samples_per_symbol < 2:
            raise TypeError, "samples_per_symbol must be >= 2, is %r" % (samples_per_symbol,)

        arity = pow(2,self.bits_per_symbol())

        # Automatic gain control
        scale = (1.0/16384.0)
        self.pre_scaler = gr.multiply_const_cc(scale)   # scale the signal from full-range to +-1
        #self.agc = gr.agc2_cc(0.6e-1, 1e-3, 1, 1, 100)
        self.agc = gr.feedforward_agc_cc(16, 2.0)

        # RRC data filter
        ntaps = 11 * samples_per_symbol
        self.rrc_taps = gr.firdes.root_raised_cosine(
            1.0,                      # gain
            self._samples_per_symbol, # sampling rate
            1.0,                      # symbol rate
            self._excess_bw,          # excess bandwidth (roll-off factor)
            ntaps)
        self.rrc_filter=gr.interp_fir_filter_ccf(1, self.rrc_taps)        

        # symbol clock recovery
        if not self._mm_gain_mu:
            self._mm_gain_mu = 0.1
            
        self._mm_omega = self._samples_per_symbol
        self._mm_gain_omega = .25 * self._mm_gain_mu * self._mm_gain_mu
        self._costas_beta  = 0.25 * self._costas_alpha * self._costas_alpha
        fmin = -0.025
        fmax = 0.025
        
        self.receiver=gr.mpsk_receiver_cc(arity, 0,
                                         self._costas_alpha, self._costas_beta,
                                         fmin, fmax,
                                         self._mm_mu, self._mm_gain_mu,
                                         self._mm_omega, self._mm_gain_omega,
                                         self._mm_omega_relative_limit)

        # Do differential decoding based on phase change of symbols
        self.diffdec = gr.diff_phasor_cc()

        # find closest constellation point
        rot = 1
        rotated_const = map(lambda pt: pt * rot, psk.constellation[arity])
        self.slicer = gr.constellation_decoder_cb(rotated_const, range(arity))

        if self._gray_code:
            self.symbol_mapper = gr.map_bb(psk.gray_to_binary[arity])
        else:
            self.symbol_mapper = gr.map_bb(psk.ungray_to_binary[arity])
        
        # unpack the k bit vector into a stream of bits
        self.unpack = gr.unpack_k_bits_bb(self.bits_per_symbol())

        if verbose:
            self._print_verbage()

        if log:
            self._setup_logging()

        # Connect and Initialize base class
        self.connect(self, self.pre_scaler, self.agc, self.rrc_filter, self.receiver,
                     self.diffdec, self.slicer, self.symbol_mapper, self.unpack, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 1
    bits_per_symbol = staticmethod(bits_per_symbol)      # make it a static method.  RTFM

    def _print_verbage(self):
        print "\nDemodulator:"
        print "bits per symbol:     %d"   % self.bits_per_symbol()
        print "Gray code:           %s"   % self._gray_code
        print "RRC roll-off factor: %.2f" % self._excess_bw
        print "Costas Loop alpha:   %.2e" % self._costas_alpha
        print "Costas Loop beta:    %.2e" % self._costas_beta
        print "M&M mu:              %.2f" % self._mm_mu
        print "M&M mu gain:         %.2e" % self._mm_gain_mu
        print "M&M omega:           %.2f" % self._mm_omega
        print "M&M omega gain:      %.2e" % self._mm_gain_omega
        print "M&M omega limit:     %.2f" % self._mm_omega_relative_limit

    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.pre_scaler,
                         gr.file_sink(gr.sizeof_gr_complex, "rx_prescaler.dat"))
        self.connect(self.agc,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_agc.dat"))
        self.connect(self.rrc_filter,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_rrc_filter.dat"))
        self.connect(self.receiver,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_receiver.dat"))
        self.connect(self.diffdec,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_diffdec.dat"))        
        self.connect(self.slicer,
                    gr.file_sink(gr.sizeof_char, "rx_slicer.dat"))
        self.connect(self.symbol_mapper,
                     gr.file_sink(gr.sizeof_char, "rx_symbol_mapper.dat"))
        self.connect(self.unpack,
                     gr.file_sink(gr.sizeof_char, "rx_unpack.dat"))
        
    def add_options(parser):
        """
        Adds DBPSK demodulation-specific options to the standard parser
        """
        parser.add_option("", "--excess-bw", type="float", default=_def_excess_bw,
                          help="set RRC excess bandwith factor [default=%default] (PSK)")
        parser.add_option("", "--no-gray-code", dest="gray_code",
                          action="store_false", default=_def_gray_code,
                          help="disable gray coding on modulated bits (PSK)")
        parser.add_option("", "--costas-alpha", type="float", default=None,
                          help="set Costas loop alpha value [default=%default] (PSK)")
        parser.add_option("", "--gain-mu", type="float", default=_def_gain_mu,
                          help="set M&M symbol sync loop gain mu value [default=%default] (GMSK/PSK)")
        parser.add_option("", "--mu", type="float", default=_def_mu,
                          help="set M&M symbol sync loop mu value [default=%default] (GMSK/PSK)")
        parser.add_option("", "--omega-relative-limit", type="float", default=_def_omega_relative_limit,
                          help="M&M clock recovery omega relative limit [default=%default] (GMSK/PSK)")
    add_options=staticmethod(add_options)
    
    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(
                 dbpsk_demod.__init__, ('self',), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)
#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('dbpsk', dbpsk_mod)
modulation_utils.add_type_1_demod('dbpsk', dbpsk_demod)
