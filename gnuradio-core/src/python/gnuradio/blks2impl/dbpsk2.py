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

_def_freq_alpha = 4e-3
_def_costas_alpha = 0.1
_def_timing_alpha = 0.100
_def_timing_beta = 0.010
_def_timing_max_dev = 1.5


# /////////////////////////////////////////////////////////////////////////////
#                             DBPSK modulator
# /////////////////////////////////////////////////////////////////////////////

class dbpsk2_mod(gr.hier_block2):

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
        return modulation_utils.extract_kwargs_from_options(dbpsk2_mod.__init__,
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

class dbpsk2_demod(gr.hier_block2):

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 excess_bw=_def_excess_bw,
                 freq_alpha=_def_freq_alpha,
                 costas_alpha=_def_costas_alpha,
                 timing_alpha=_def_timing_alpha,
                 timing_max_dev=_def_timing_max_dev,
                 gray_code=_def_gray_code,
                 verbose=_def_verbose,
                 log=_def_log,
                 sync_out=False):
        """
	Hierarchical block for RRC-filtered differential BPSK demodulation

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (LSB)

	@param samples_per_symbol: samples per symbol >= 2
	@type samples_per_symbol: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param freq_alpha: loop filter gain for frequency recovery
        @type freq_alpha: float
        @param costas_alpha: loop filter gain for phase/fine frequency recovery
        @type costas_alpha: float
        @param timing_alpha: loop alpha gain for timing recovery
        @type timing_alpha: float
        @param timing_max: timing loop maximum rate deviations
        @type timing_max: float
        @param gray_code: Tell modulator to Gray code the bits
        @type gray_code: bool
        @param verbose: Print information about modulator?
        @type verbose: bool
        @param log: Print modualtion data to files?
        @type log: bool
        @param sync_out: Output a sync signal on :1?
        @type sync_out: bool
	"""
	if sync_out: io_sig_out = gr.io_signaturev(2, 2, (gr.sizeof_char, gr.sizeof_gr_complex))
	else: io_sig_out = gr.io_signature(1, 1, gr.sizeof_char)

	gr.hier_block2.__init__(self, "dqpsk2_demod",
			        gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
			        io_sig_out)       # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._excess_bw = excess_bw
        self._freq_alpha = freq_alpha
        self._freq_beta = 0.25*self._freq_alpha**2
        self._costas_alpha = costas_alpha
        self._timing_alpha = timing_alpha
        self._timing_beta = _def_timing_beta
        self._timing_max_dev=timing_max_dev
        self._gray_code = gray_code
        
        if samples_per_symbol < 2:
            raise TypeError, "samples_per_symbol must be >= 2, is %r" % (samples_per_symbol,)

        arity = pow(2,self.bits_per_symbol())

        # Automatic gain control
        self.agc = gr.agc2_cc(0.6e-1, 1e-3, 1, 1, 100)
        #self.agc = gr.feedforward_agc_cc(16, 1.0)


        # Frequency correction
        self.freq_recov = gr.fll_band_edge_cc(self._samples_per_symbol, self._excess_bw,
                                              11*self._samples_per_symbol,
                                              self._freq_alpha, self._freq_beta)
                                              

        # symbol timing recovery with RRC data filter
        nfilts = 32
        ntaps = 11 * samples_per_symbol*nfilts
        taps = gr.firdes.root_raised_cosine(nfilts, nfilts,
                                            1.0/float(self._samples_per_symbol),
                                            self._excess_bw, ntaps)
        self.time_recov = gr.pfb_clock_sync_ccf(self._samples_per_symbol,
                                                self._timing_alpha,
                                                taps, nfilts, nfilts/2, self._timing_max_dev)
        self.time_recov.set_beta(self._timing_beta)

        # Perform phase / fine frequency correction
        self._costas_beta  = 0.25 * self._costas_alpha * self._costas_alpha
        # Allow a frequency swing of +/- half of the sample rate
        fmin = -0.5
        fmax = 0.5
        
        self.phase_recov = gr.costas_loop_cc(self._costas_alpha,
                                             self._costas_beta,
                                             fmax, fmin, arity)

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

        # Connect
        self.connect(self, self.agc,
                     self.freq_recov, self.time_recov, self.phase_recov,
                     self.diffdec, self.slicer, self.symbol_mapper, self.unpack, self)
        if sync_out: self.connect(self.time_recov, (self, 1))

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
        print "FLL gain:            %.2f" % self._freq_alpha
        print "Costas Loop alpha:   %.2f" % self._costas_alpha
        print "Costas Loop beta:    %.2f" % self._costas_beta
        print "Timing alpha gain:   %.2f" % self._timing_alpha
        print "Timing beta gain:    %.2f" % self._timing_beta
        print "Timing max dev:      %.2f" % self._timing_max_dev

    def _setup_logging(self):
        print "Modulation logging turned on."
        self.connect(self.pre_scaler,
                         gr.file_sink(gr.sizeof_gr_complex, "rx_prescaler.dat"))
        self.connect(self.agc,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_agc.dat"))
        self.connect(self.rrc_filter,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_rrc_filter.dat"))
        self.connect(self.clock_recov,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_clock_recov.dat"))
        self.connect(self.time_recov,
                     gr.file_sink(gr.sizeof_gr_complex, "rx_time_recov.dat"))
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
        parser.add_option("", "--freq-alpha", type="float", default=_def_freq_alpha,
                          help="set frequency lock loop alpha gain value [default=%default] (PSK)")
        parser.add_option("", "--costas-alpha", type="float", default=None,
                          help="set Costas loop alpha value [default=%default] (PSK)")
        parser.add_option("", "--gain-alpha", type="float", default=_def_timing_alpha,
                          help="set timing symbol sync loop gain alpha value [default=%default] (GMSK/PSK)")
        parser.add_option("", "--gain-beta", type="float", default=_def_timing_beta,
                          help="set timing symbol sync loop gain beta value [default=%default] (GMSK/PSK)")
        parser.add_option("", "--timing-max-dev", type="float", default=_def_timing_max_dev,
                          help="set timing symbol sync loop maximum deviation [default=%default] (GMSK/PSK)")
    add_options=staticmethod(add_options)
    
    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(
                 dbpsk2_demod.__init__, ('self',), options)
    extract_kwargs_from_options=staticmethod(extract_kwargs_from_options)
#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('dbpsk2', dbpsk2_mod)
modulation_utils.add_type_1_demod('dbpsk2', dbpsk2_demod)
