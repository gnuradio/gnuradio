#
# Copyright 2005,2006,2007,2009,2011 Free Software Foundation, Inc.
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
Generic modulation and demodulation.
"""

from gnuradio import gr
from gnuradio.modulation_utils2 import extract_kwargs_from_options_for_class
#from gnuradio.digital.utils import mod_codes
from utils import mod_codes
import digital_swig

# default values (used in __init__ and add_options)
_def_samples_per_symbol = 2
_def_excess_bw = 0.35
_def_verbose = False
_def_log = False

# Frequency correction
_def_freq_alpha = 0.010
# Symbol timing recovery 
_def_timing_alpha = 0.100
_def_timing_beta = 0.010
_def_timing_max_dev = 1.5
# Fine frequency / Phase correction
_def_phase_alpha = 0.1
# Number of points in constellation
_def_constellation_points = 16
# Whether differential coding is used.
_def_differential = True

def add_common_options(parser):
    """
    Sets options common to both modulator and demodulator.
    """
    parser.add_option("-p", "--constellation-points", type="int", default=_def_constellation_points,
                      help="set the number of constellation points (must be a power of 2 (power of 4 for QAM) [default=%default]")
    parser.add_option("", "--differential", action="store_true", dest="differential", default=True,
                      help="use differential encoding [default=%default]")
    parser.add_option("", "--not-differential", action="store_false", dest="differential",
                      help="do not use differential encoding [default=%default]")
    parser.add_option("", "--mod-code", type="choice", choices=mod_codes.codes,
                      default=mod_codes.NO_CODE,
                      help="Select modulation code from: %s [default=%%default]"
                            % (', '.join(mod_codes.codes),))
    parser.add_option("", "--excess-bw", type="float", default=_def_excess_bw,
                      help="set RRC excess bandwith factor [default=%default]")
    

# /////////////////////////////////////////////////////////////////////////////
#                             Generic modulator
# /////////////////////////////////////////////////////////////////////////////

class generic_mod(gr.hier_block2):

    def __init__(self, constellation,
                 differential=_def_differential,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for RRC-filtered differential generic modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.
        
	@param constellation: determines the modulation type
	@type constellation: gnuradio.digital.gr_constellation
	@param samples_per_symbol: samples per baud >= 2
	@type samples_per_symbol: integer
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Log modulation data to files?
        @type log: bool
	"""

	gr.hier_block2.__init__(self, "generic_mod",
				gr.io_signature(1, 1, gr.sizeof_char),       # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._constellation = constellation.base()
        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._differential = differential
 
        if not isinstance(self._samples_per_symbol, int) or self._samples_per_symbol < 2:
            raise TypeError, ("sbp must be an integer >= 2, is %d" % self._samples_per_symbol)
        
	ntaps = 11 * self._samples_per_symbol

        arity = pow(2,self.bits_per_symbol())
        
        # turn bytes into k-bit vectors
        self.bytes2chunks = \
          gr.packed_to_unpacked_bb(self.bits_per_symbol(), gr.GR_MSB_FIRST)

        if self._constellation.apply_pre_diff_code():
            self.symbol_mapper = gr.map_bb(self._constellation.pre_diff_code())

        if differential:
            self.diffenc = gr.diff_encoder_bb(arity)

        self.chunks2symbols = gr.chunks_to_symbols_bc(self._constellation.points())

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
        blocks = [self, self.bytes2chunks]
        if self._constellation.apply_pre_diff_code():
            blocks.append(self.symbol_mapper)
        if differential:
            blocks.append(self.diffenc)
        blocks += [self.chunks2symbols, self.rrc_filter, self]
        self.connect(*blocks)

        if verbose:
            self._print_verbage()
            
        if log:
            self._setup_logging()
            

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self):   # static method that's also callable on an instance
        return self._constellation.bits_per_symbol()

    def add_options(parser):
        """
        Adds generic modulation options to the standard parser
        """
        add_common_options(parser)
    add_options=staticmethod(add_options)

    def extract_kwargs_from_options(cls, options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return extract_kwargs_from_options_for_class(cls, options)
    extract_kwargs_from_options=classmethod(extract_kwargs_from_options)


    def _print_verbage(self):
        print "\nModulator:"
        print "bits per symbol:     %d" % self.bits_per_symbol()
        print "RRC roll-off factor: %.2f" % self._excess_bw

    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.bytes2chunks,
                     gr.file_sink(gr.sizeof_char, "tx_bytes2chunks.dat"))
        if self._constellation.apply_pre_diff_code():
            self.connect(self.symbol_mapper,
                         gr.file_sink(gr.sizeof_char, "tx_symbol_mapper.dat"))
        if self._differential:
            self.connect(self.diffenc,
                         gr.file_sink(gr.sizeof_char, "tx_diffenc.dat"))
        self.connect(self.chunks2symbols,
                     gr.file_sink(gr.sizeof_gr_complex, "tx_chunks2symbols.dat"))
        self.connect(self.rrc_filter,
                     gr.file_sink(gr.sizeof_gr_complex, "tx_rrc_filter.dat"))
              

# /////////////////////////////////////////////////////////////////////////////
#                             Generic demodulator
#
#      Differentially coherent detection of differentially encoded generically
#      modulated signal.
# /////////////////////////////////////////////////////////////////////////////

class generic_demod(gr.hier_block2):

    def __init__(self, constellation,
                 samples_per_symbol=_def_samples_per_symbol,
                 differential=_def_differential,
                 excess_bw=_def_excess_bw,
                 freq_alpha=_def_freq_alpha,
                 timing_alpha=_def_timing_alpha,
                 timing_max_dev=_def_timing_max_dev,
                 phase_alpha=_def_phase_alpha,
                 verbose=_def_verbose,
                 log=_def_log):
        """
	Hierarchical block for RRC-filtered differential generic demodulation.

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (LSB)

	@param constellation: determines the modulation type
	@type constellation: gnuradio.digital.gr_constellation
	@param samples_per_symbol: samples per symbol >= 2
	@type samples_per_symbol: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param freq_alpha: loop filter gain for frequency recovery
        @type freq_alpha: float
        @param timing_alpha: loop alpha gain for timing recovery
        @type timing_alpha: float
        @param timing_max_dev: timing loop maximum rate deviations
        @type timing_max_dev: float
        @param phase_alpha: loop filter gain in phase loop
        @type phase_alphas: float
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param debug: Print modualtion data to files?
        @type debug: bool
	"""
        
	gr.hier_block2.__init__(self, "generic_demod",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_char))       # Output signature
				
        self._constellation = constellation.base()
        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._phase_alpha = phase_alpha
        self._freq_alpha = freq_alpha
        self._freq_beta = 0.10*self._freq_alpha
        self._timing_alpha = timing_alpha
        self._timing_beta = _def_timing_beta
        self._timing_max_dev=timing_max_dev
        self._differential = differential

        if not isinstance(self._samples_per_symbol, int) or self._samples_per_symbol < 2:
            raise TypeError, ("sbp must be an integer >= 2, is %d" % self._samples_per_symbol)

        arity = pow(2,self.bits_per_symbol())

        # Automatic gain control
        self.agc = gr.agc2_cc(0.6e-1, 1e-3, 1, 1, 100)

        # Frequency correction
        self.freq_recov = gr.fll_band_edge_cc(self._samples_per_symbol, self._excess_bw,
                                              11*int(self._samples_per_symbol),
                                              self._freq_alpha, self._freq_beta)

        # symbol timing recovery with RRC data filter
        nfilts = 32
        ntaps = 11 * int(self._samples_per_symbol*nfilts)
        taps = gr.firdes.root_raised_cosine(nfilts, nfilts,
                                            1.0/float(self._samples_per_symbol),
                                            self._excess_bw, ntaps)
        self.time_recov = gr.pfb_clock_sync_ccf(self._samples_per_symbol,
                                                self._timing_alpha,
                                                taps, nfilts, nfilts/2, self._timing_max_dev)
        self.time_recov.set_beta(self._timing_beta)

        #self._phase_beta  = 0.25 * self._phase_alpha * self._phase_alpha
        self._phase_beta  = 0.25 * self._phase_alpha * self._phase_alpha
        fmin = -0.25
        fmax = 0.25
        
        self.receiver = digital_swig.constellation_receiver_cb(
            self._constellation,
            self._phase_alpha, self._phase_beta,
            fmin, fmax)
        
        # Do differential decoding based on phase change of symbols
        if differential:
            self.diffdec = gr.diff_decoder_bb(arity)

        if self._constellation.apply_pre_diff_code():
            self.symbol_mapper = gr.map_bb(
                mod_codes.invert_code(self._constellation.pre_diff_code()))

        # unpack the k bit vector into a stream of bits
        self.unpack = gr.unpack_k_bits_bb(self.bits_per_symbol())

        if verbose:
            self._print_verbage()

        if log:
            self._setup_logging()

        # Connect and Initialize base class
        blocks = [self, self.agc, self.freq_recov, self.time_recov, self.receiver]
        if differential:
            blocks.append(self.diffdec)
        if self._constellation.apply_pre_diff_code():
            blocks.append(self.symbol_mapper)
        blocks += [self.unpack, self]
        self.connect(*blocks)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    def bits_per_symbol(self):   # staticmethod that's also callable on an instance
        return self._constellation.bits_per_symbol()

    def _print_verbage(self):
        print "\nDemodulator:"
        print "bits per symbol:     %d"   % self.bits_per_symbol()
        print "RRC roll-off factor: %.2f" % self._excess_bw
        print "FLL gain:            %.2e" % self._freq_alpha
        print "Timing alpha gain:   %.2e" % self._timing_alpha
        print "Timing beta gain:    %.2e" % self._timing_beta
        print "Timing max dev:      %.2f" % self._timing_max_dev
        print "Phase track alpha:   %.2e" % self._phase_alpha
        print "Phase track beta:    %.2e" % self._phase_beta

    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.agc,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_agc.dat"))
        self.connect((self.freq_recov, 0),
                     gr.file_sink(gr.sizeof_gr_complex, "rx_freq_recov.dat"))
        self.connect((self.freq_recov, 1),
                     gr.file_sink(gr.sizeof_float, "rx_freq_recov_freq.dat"))
        self.connect((self.freq_recov, 2),
                     gr.file_sink(gr.sizeof_float, "rx_freq_recov_phase.dat"))
        self.connect((self.freq_recov, 3),
                     gr.file_sink(gr.sizeof_gr_complex, "rx_freq_recov_error.dat"))
        self.connect((self.time_recov, 0),
                     gr.file_sink(gr.sizeof_gr_complex, "rx_time_recov.dat"))
        self.connect((self.time_recov, 1),
                     gr.file_sink(gr.sizeof_float, "rx_time_recov_error.dat"))
        self.connect((self.time_recov, 2),
                     gr.file_sink(gr.sizeof_float, "rx_time_recov_rate.dat"))
        self.connect((self.time_recov, 3),
                     gr.file_sink(gr.sizeof_float, "rx_time_recov_phase.dat"))
        self.connect((self.receiver, 0),
                     gr.file_sink(gr.sizeof_char, "rx_receiver.dat"))
        self.connect((self.receiver, 1),
                     gr.file_sink(gr.sizeof_float, "rx_receiver_error.dat"))
        self.connect((self.receiver, 2),
                     gr.file_sink(gr.sizeof_float, "rx_receiver_phase.dat"))
        self.connect((self.receiver, 3),
                     gr.file_sink(gr.sizeof_float, "rx_receiver_freq.dat"))
        if self._differential:
            self.connect(self.diffdec,
                         gr.file_sink(gr.sizeof_char, "rx_diffdec.dat"))        
        if self._constellation.apply_pre_diff_code():
            self.connect(self.symbol_mapper,
                         gr.file_sink(gr.sizeof_char, "rx_symbol_mapper.dat"))        
        self.connect(self.unpack,
                     gr.file_sink(gr.sizeof_char, "rx_unpack.dat"))
        
    def add_options(parser):
        """
        Adds generic demodulation options to the standard parser
        """
        # Add options shared with modulator.
        add_common_options(parser)
        # Add options specific to demodulator.
        parser.add_option("", "--freq-alpha", type="float", default=_def_freq_alpha,
                          help="set frequency lock loop alpha gain value [default=%default]")
        parser.add_option("", "--phase-alpha", type="float", default=_def_phase_alpha,
                          help="set phase tracking loop alpha value [default=%default]")
        parser.add_option("", "--timing-alpha", type="float", default=_def_timing_alpha,
                          help="set timing symbol sync loop gain alpha value [default=%default]")
        parser.add_option("", "--timing-beta", type="float", default=_def_timing_beta,
                          help="set timing symbol sync loop gain beta value [default=%default]")
        parser.add_option("", "--timing-max-dev", type="float", default=_def_timing_max_dev,
                          help="set timing symbol sync loop maximum deviation [default=%default]")
    add_options=staticmethod(add_options)
    
    def extract_kwargs_from_options(cls, options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return extract_kwargs_from_options_for_class(cls, options)
    extract_kwargs_from_options=classmethod(extract_kwargs_from_options)

