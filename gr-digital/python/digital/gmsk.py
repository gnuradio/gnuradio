#
# GMSK modulation and demodulation.
#
#
# Copyright 2005-2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


# See gnuradio-examples/python/digital for examples

from math import pi
from pprint import pprint
import inspect

import numpy

from gnuradio import gr, blocks, analog, filter
from . import modulation_utils
from . import digital_python as digital

# default values (used in __init__ and add_options)
_def_samples_per_symbol = 2
_def_bt = 0.35
_def_verbose = False
_def_log = False

_def_gain_mu = None
_def_mu = 0.5
_def_freq_error = 0.0
_def_omega_relative_limit = 0.005


# FIXME: Figure out how to make GMSK work with pfb_arb_resampler_fff for both
# transmit and receive so we don't require integer samples per symbol.


# /////////////////////////////////////////////////////////////////////////////
#                              GMSK modulator
# /////////////////////////////////////////////////////////////////////////////

class gmsk_mod(gr.hier_block2):
    """
    Hierarchical block for Gaussian Minimum Shift Key (GMSK)
    modulation.

    The input is a byte stream (unsigned char with packed bits)
    and the output is the complex modulated signal at baseband.

    Args:
        samples_per_symbol: samples per baud >= 2 (integer)
        bt: Gaussian filter bandwidth * symbol time (float)
        verbose: Print information about modulator? (boolean)
        log: Print modulation data to files? (boolean)
    """

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 bt=_def_bt,
                 verbose=_def_verbose,
                 log=_def_log):

        gr.hier_block2.__init__(self, "gmsk_mod",
                                gr.io_signature(1, 1, gr.sizeof_char),       # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        samples_per_symbol = int(samples_per_symbol)
        self._samples_per_symbol = samples_per_symbol
        self._bt = bt
        self._differential = False

        if not isinstance(samples_per_symbol, int) or samples_per_symbol < 2:
            raise TypeError("samples_per_symbol must be an integer >= 2, is %r" % (samples_per_symbol,))

        ntaps = 4 * samples_per_symbol                        # up to 3 bits in filter at once
        sensitivity = (pi / 2) / samples_per_symbol        # phase change per bit = pi / 2

        # Turn it into NRZ data.
        #self.nrz = digital.bytes_to_syms()
        self.unpack = blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
        self.nrz = digital.chunks_to_symbols_bf([-1, 1], 1)

        # Form Gaussian filter
        # Generate Gaussian response (Needs to be convolved with window below).
        self.gaussian_taps = filter.firdes.gaussian(
                1,                       # gain
                samples_per_symbol,    # symbol_rate
                bt,                       # bandwidth * symbol time
                ntaps                       # number of taps
                )

        self.sqwave = (1,) * samples_per_symbol       # rectangular window
        self.taps = numpy.convolve(numpy.array(self.gaussian_taps),numpy.array(self.sqwave))
        self.gaussian_filter = filter.interp_fir_filter_fff(samples_per_symbol, self.taps)

        # FM modulation
        self.fmmod = analog.frequency_modulator_fc(sensitivity)

        if verbose:
            self._print_verbage()

        if log:
            self._setup_logging()

        # Connect & Initialize base class
        self.connect(self, self.unpack, self.nrz, self.gaussian_filter, self.fmmod, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    @staticmethod
    def bits_per_symbol(self=None):     # staticmethod that's also callable on an instance
        return 1

    def _print_verbage(self):
        print("bits per symbol = %d" % self.bits_per_symbol())
        print("Gaussian filter bt = %.2f" % self._bt)


    def _setup_logging(self):
        print("Modulation logging turned on.")
        self.connect(self.nrz,
                     blocks.file_sink(gr.sizeof_float, "nrz.dat"))
        self.connect(self.gaussian_filter,
                     blocks.file_sink(gr.sizeof_float, "gaussian_filter.dat"))
        self.connect(self.fmmod,
                     blocks.file_sink(gr.sizeof_gr_complex, "fmmod.dat"))

    @staticmethod
    def add_options(parser):
        """
        Adds GMSK modulation-specific options to the standard parser
        """
        parser.add_option("", "--bt", type="float", default=_def_bt,
                          help="set bandwidth-time product [default=%default] (GMSK)")

    @staticmethod
    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(gmsk_mod.__init__,
                                                            ('self',), options)


# /////////////////////////////////////////////////////////////////////////////
#                            GMSK demodulator
# /////////////////////////////////////////////////////////////////////////////

class gmsk_demod(gr.hier_block2):
    """
    Hierarchical block for Gaussian Minimum Shift Key (GMSK)
    demodulation.

    The input is the complex modulated signal at baseband.
    The output is a stream of bits packed 1 bit per byte (the LSB)

    Args:
        samples_per_symbol: samples per baud (integer)
        gain_mu: controls rate of mu adjustment (float)
        mu: fractional delay [0.0, 1.0] (float)
        omega_relative_limit: sets max variation in omega (float)
        freq_error: bit rate error as a fraction (float)
        verbose: Print information about modulator? (boolean)
        log: Print modualtion data to files? (boolean)
    """

    def __init__(self,
                 samples_per_symbol=_def_samples_per_symbol,
                 gain_mu=_def_gain_mu,
                 mu=_def_mu,
                 omega_relative_limit=_def_omega_relative_limit,
                 freq_error=_def_freq_error,
                 verbose=_def_verbose,
                 log=_def_log):

        gr.hier_block2.__init__(self, "gmsk_demod",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_char))       # Output signature

        self._samples_per_symbol = samples_per_symbol
        self._gain_mu = gain_mu
        self._mu = mu
        self._omega_relative_limit = omega_relative_limit
        self._freq_error = freq_error
        self._differential = False

        if samples_per_symbol < 2:
            raise TypeError("samples_per_symbol >= 2, is %f" % samples_per_symbol)

        self._omega = samples_per_symbol*(1+self._freq_error)

        if not self._gain_mu:
            self._gain_mu = 0.175

        self._gain_omega = .25 * self._gain_mu * self._gain_mu        # critically damped

        # Demodulate FM
        sensitivity = (pi / 2) / samples_per_symbol
        self.fmdemod = analog.quadrature_demod_cf(1.0 / sensitivity)

        # the clock recovery block tracks the symbol clock and resamples as needed.
        # the output of the block is a stream of soft symbols (float)
        self.clock_recovery = digital.clock_recovery_mm_ff(self._omega, self._gain_omega,
                                                           self._mu, self._gain_mu,
                                                           self._omega_relative_limit)

        # slice the floats at 0, outputting 1 bit (the LSB of the output byte) per sample
        self.slicer = digital.binary_slicer_fb()

        if verbose:
            self._print_verbage()

        if log:
            self._setup_logging()

        # Connect & Initialize base class
        self.connect(self, self.fmdemod, self.clock_recovery, self.slicer, self)

    def samples_per_symbol(self):
        return self._samples_per_symbol

    @staticmethod
    def bits_per_symbol(self=None):   # staticmethod that's also callable on an instance
        return 1

    def _print_verbage(self):
        print("bits per symbol = %d" % self.bits_per_symbol())
        print("M&M clock recovery omega = %f" % self._omega)
        print("M&M clock recovery gain mu = %f" % self._gain_mu)
        print("M&M clock recovery mu = %f" % self._mu)
        print("M&M clock recovery omega rel. limit = %f" % self._omega_relative_limit)
        print("frequency error = %f" % self._freq_error)


    def _setup_logging(self):
        print("Demodulation logging turned on.")
        self.connect(self.fmdemod,
                    blocks.file_sink(gr.sizeof_float, "fmdemod.dat"))
        self.connect(self.clock_recovery,
                    blocks.file_sink(gr.sizeof_float, "clock_recovery.dat"))
        self.connect(self.slicer,
                    blocks.file_sink(gr.sizeof_char, "slicer.dat"))

    @staticmethod
    def add_options(parser):
        """
        Adds GMSK demodulation-specific options to the standard parser
        """
        parser.add_option("", "--gain-mu", type="float", default=_def_gain_mu,
                          help="M&M clock recovery gain mu [default=%default] (GMSK/PSK)")
        parser.add_option("", "--mu", type="float", default=_def_mu,
                          help="M&M clock recovery mu [default=%default] (GMSK/PSK)")
        parser.add_option("", "--omega-relative-limit", type="float", default=_def_omega_relative_limit,
                          help="M&M clock recovery omega relative limit [default=%default] (GMSK/PSK)")
        parser.add_option("", "--freq-error", type="float", default=_def_freq_error,
                          help="M&M clock recovery frequency error [default=%default] (GMSK)")

    @staticmethod
    def extract_kwargs_from_options(options):
        """
        Given command line options, create dictionary suitable for passing to __init__
        """
        return modulation_utils.extract_kwargs_from_options(gmsk_demod.__init__,
                                                            ('self',), options)


#
# Add these to the mod/demod registry
#
modulation_utils.add_type_1_mod('gmsk', gmsk_mod)
modulation_utils.add_type_1_demod('gmsk', gmsk_demod)
