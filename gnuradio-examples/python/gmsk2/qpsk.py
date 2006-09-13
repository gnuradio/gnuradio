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

"""
PSK and differential PSK modulation and demodulation.
"""

from gnuradio import gr, gru
from math import pi, sqrt
import cmath
import Numeric
from pprint import pprint

_use_gray_code = True

def make_constellation(m):
    return [cmath.exp(i * 2 * pi / m * 1j) for i in range(m)]
        
# Common definition of constellations for Tx and Rx
constellation = {
    2 : make_constellation(2),           # BPSK
    4 : make_constellation(4),           # QPSK
    8 : make_constellation(8)            # 8PSK
    }

if 0:
    print "const(2) ="
    pprint(constellation[2])
    print "const(4) ="
    pprint(constellation[4])
    print "const(8) ="
    pprint(constellation[8])


if _use_gray_code:
    # -----------------------
    # Do Gray code
    # -----------------------
    # binary to gray coding
    binary_to_gray = {
        2 : (0, 1),
        4 : (0, 1, 3, 2),
        8 : (0, 1, 3, 2, 7, 6, 4, 5)
        }
    
    # gray to binary
    gray_to_binary = {
        2 : (0, 1),
        4 : (0, 1, 3, 2),
        8 : (0, 1, 3, 2, 6, 7, 5, 4)
        }
else:
    # -----------------------
    # Don't Gray code
    # -----------------------
    # identity mapping
    binary_to_gray = {
        2 : (0, 1),
        4 : (0, 1, 2, 3),
        8 : (0, 1, 2, 3, 4, 5, 6, 7)
        }
    
    # identity mapping
    gray_to_binary = {
        2 : (0, 1),
        4 : (0, 1, 2, 3),
        8 : (0, 1, 2, 3, 4, 5, 6, 7)
        }


# /////////////////////////////////////////////////////////////////////////////
#            mPSK mod/demod with steams of bytes as data i/o
# /////////////////////////////////////////////////////////////////////////////


class mpsk_mod(gr.hier_block):

    def __init__(self, fg, spb, arity, excess_bw, diff=False):
        """
	Hierarchical block for RRC-filtered PSK modulation.

	The input is a byte stream (unsigned char) and the
	output is the complex modulated signal at baseband.

	@param fg: flow graph
	@type fg: flow graph
	@param spb: samples per baud >= 2
	@type spb: integer
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param arity: whick PSK: 2, 4, 8
        @type arity: int in {2, 4, 8}
        @param diff: differential PSK if true
        @type diff: bool
	"""
        if not isinstance(spb, int) or spb < 2:
            raise TypeError, "sbp must be an integer >= 2"
        self.spb = spb

        if not arity in (2, 4):
            raise ValueError, "n must be 2, 4, or 8"

	ntaps = 11 * spb

        bits_per_symbol = int(gru.log2(arity))
        self.bits_per_symbol = bits_per_symbol
        print "bits_per_symbol =", bits_per_symbol
        
        # turn bytes into k-bit vectors
        self.bytes2chunks = \
          gr.packed_to_unpacked_bb(bits_per_symbol, gr.GR_MSB_FIRST)

        if True or arity > 2:
            self.gray_coder = gr.map_bb(binary_to_gray[arity])
        else:
            self.gray_coder = None

        if diff:
            self.diffenc = gr.diff_encoder_bb(arity)
        else:
            self.diffenc = None

        self.chunks2symbols = gr.chunks_to_symbols_bc(constellation[arity])

        # pulse shaping filter
	self.rrc_taps = gr.firdes.root_raised_cosine(
		spb,		# gain  (spb since we're interpolating by spb)
		spb,            # sampling rate
		1.0,		# symbol rate
		excess_bw,      # excess bandwidth (roll-off factor)
                ntaps)

	self.rrc_filter = gr.interp_fir_filter_ccf(spb, self.rrc_taps)

	# Connect
        if self.gray_coder:
            fg.connect(self.bytes2chunks, self.gray_coder)
            t = self.gray_coder
        else:
            t = self.bytes2chunks

        if diff:
            fg.connect(t, self.diffenc, self.chunks2symbols, self.rrc_filter)
        else:
            fg.connect(t, self.chunks2symbols, self.rrc_filter)

        if 1:
            fg.connect(self.gray_coder,
                       gr.file_sink(gr.sizeof_char, "graycoder.dat"))
            if diff:
                fg.connect(self.diffenc,
                           gr.file_sink(gr.sizeof_char, "diffenc.dat"))
            
	# Initialize base class
	gr.hier_block.__init__(self, fg, self.bytes2chunks, self.rrc_filter)

    def samples_per_baud(self):
        return self.spb

    def bits_per_baud(self):
        return self.bits_per_symbol


class mpsk_demod__coherent_detection_of_differentially_encoded_psk(gr.hier_block):
    def __init__(self, fg, spb, arity, excess_bw, diff=False, costas_alpha=0.005, gain_mu=0.05):
        """
	Hierarchical block for RRC-filtered PSK demodulation

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (LSB)

	@param fg: flow graph
	@type fg: flow graph
	@param spb: samples per baud >= 2
	@type spb: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param arity: whick PSK: 2, 4, 8
        @type arity: int in {2, 4, 8}
        @param diff: differential PSK if true
        @type diff: bool
        @param costas_alpha: loop filter gain
        @type costas_alphas: float
        @param gain_mu:
        @type gain_mu: float
	"""
        if spb < 2:
            raise TypeError, "sbp must be >= 2"
        self.spb = spb

        if not arity in (2, 4):
            raise ValueError, "n must be 2 or 4"

        if not diff and arity==4:
            raise NotImplementedError, "non-differential QPSK not supported yet"

        bits_per_symbol = int(gru.log2(arity))
        print "bits_per_symbol =", bits_per_symbol

        # Automatic gain control
        self.agc = gr.agc_cc(1e-3, 1, 1)
        
        # Costas loop (carrier tracking)
        # FIXME: need to decide how to handle this more generally; do we pull it from higher layer?
        if arity == 2:
            costas_order = 2
            costas_alpha *= 15   # 2nd order loop needs more gain
        else:
            costas_order = 4
        beta = .25 * costas_alpha * costas_alpha
        self.costas_loop = gr.costas_loop_cc(costas_alpha, beta, 0.05, -0.05, costas_order)

        # RRC data filter
        ntaps = 11 * spb
        self.rrc_taps = gr.firdes.root_raised_cosine(
            1.0,                # gain 
            spb,                # sampling rate
            1.0,                # symbol rate
            excess_bw,          # excess bandwidth (roll-off factor)
            ntaps)

        self.rrc_filter=gr.fir_filter_ccf(1, self.rrc_taps)

        # symbol clock recovery
        omega = spb
        gain_omega = .25 * gain_mu * gain_mu
        omega_rel_limit = 0.5
        mu = 0.05
        gain_mu = 0.1
        self.clock_recovery=gr.clock_recovery_mm_cc(omega, gain_omega,
                                                    mu, gain_mu, omega_rel_limit)

        # find closest constellation point
        #rot = .707 + .707j
        rot = 1
        rotated_const = map(lambda pt: pt * rot, constellation[arity])
        print "rotated_const =", rotated_const

        if(diff):
            self.diffdec = gr.diff_phasor_cc()
            #self.diffdec = gr.diff_decoder_bb(arity)

        self.slicer = gr.constellation_decoder_cb(rotated_const, range(arity))
        self.gray_decoder = gr.map_bb(gray_to_binary[arity])
        
        # unpack the k bit vector into a stream of bits
        self.unpack = gr.unpack_k_bits_bb(bits_per_symbol)

        if(diff):
            fg.connect(self.agc, self.costas_loop, self.rrc_filter, self.clock_recovery,
                       self.diffdec, self.slicer, self.gray_decoder, self.unpack)
        else:
            fg.connect(self.agc, self.costas_loop, self.rrc_filter, self.clock_recovery,
                       self.slicer, self.gray_decoder, self.unpack)

        #fg.connect(self.agc, self.costas_loop, self.rrc_filter, self.clock_recovery,
        #           self.slicer, self.diffdec, self.gray_decoder, self.unpack)
        
        # Debug sinks
        if 1:
            fg.connect(self.agc,
                       gr.file_sink(gr.sizeof_gr_complex, "agc.dat"))
            fg.connect(self.costas_loop,
                       gr.file_sink(gr.sizeof_gr_complex, "costas_loop.dat"))
            fg.connect(self.rrc_filter,
                       gr.file_sink(gr.sizeof_gr_complex, "rrc.dat"))
            fg.connect(self.clock_recovery,
                       gr.file_sink(gr.sizeof_gr_complex, "clock_recovery.dat"))
            fg.connect(self.slicer,
                       gr.file_sink(gr.sizeof_char, "slicer.dat"))
            if(diff):
                fg.connect(self.diffdec,
                           gr.file_sink(gr.sizeof_gr_complex, "diffdec.dat"))
                #fg.connect(self.diffdec,
                #          gr.file_sink(gr.sizeof_char, "diffdec.dat"))
            fg.connect(self.unpack,
                       gr.file_sink(gr.sizeof_char, "unpack.dat"))

        # Initialize base class
        gr.hier_block.__init__(self, fg, self.agc, self.unpack)

    def samples_per_baud(self):
        return self.spb

    def bits_per_baud(self):
        return self.bits_per_symbol


#########################################################################

class mpsk_demod__coherent_detection_of_nondifferentially_encoded_psk(gr.hier_block):
    def __init__(self, fg, spb, arity, excess_bw, diff=False, costas_alpha=0.005, gain_mu=0.05):
        """
	Hierarchical block for RRC-filtered PSK demodulation

	The input is the complex modulated signal at baseband.
	The output is a stream of bits packed 1 bit per byte (LSB)

	@param fg: flow graph
	@type fg: flow graph
	@param spb: samples per baud >= 2
	@type spb: float
	@param excess_bw: Root-raised cosine filter excess bandwidth
	@type excess_bw: float
        @param arity: whick PSK: 2, 4, 8
        @type arity: int in {2, 4, 8}
        @param diff: differential PSK if true
        @type diff: bool
        @param costas_alpha: loop filter gain
        @type costas_alphas: float
        @param gain_mu:
        @type gain_mu: float
	"""
        if spb < 2:
            raise TypeError, "sbp must be >= 2"
        self.spb = spb

        if not arity in (2, 4):
            raise ValueError, "n must be 2 or 4"

        bits_per_symbol = int(gru.log2(arity))
        print "bits_per_symbol =", bits_per_symbol

        # Automatic gain control
        self.agc = gr.agc_cc(1e-3, 1, 1)
        
        # Costas loop (carrier tracking)
        # FIXME: need to decide how to handle this more generally; do we pull it from higher layer?
        if arity == 2:
            costas_order = 2
            costas_alpha *= 15   # 2nd order loop needs more gain
        else:
            costas_order = 4
        beta = .25 * costas_alpha * costas_alpha
        self.costas_loop = gr.costas_loop_cc(costas_alpha, beta, 0.05, -0.05, costas_order)

        # RRC data filter
        ntaps = 11 * spb
        self.rrc_taps = gr.firdes.root_raised_cosine(
            1.0,                # gain 
            spb,                # sampling rate
            1.0,                # symbol rate
            excess_bw,          # excess bandwidth (roll-off factor)
            ntaps)

        self.rrc_filter=gr.fir_filter_ccf(1, self.rrc_taps)

        # symbol clock recovery
        omega = spb
        gain_omega = .25 * gain_mu * gain_mu
        omega_rel_limit = 0.5
        mu = 0.05
        gain_mu = 0.1
        self.clock_recovery=gr.clock_recovery_mm_cc(omega, gain_omega,
                                                    mu, gain_mu, omega_rel_limit)

        # find closest constellation point
        #rot = .707 + .707j
        rot = 1
        rotated_const = map(lambda pt: pt * rot, constellation[arity])
        print "rotated_const =", rotated_const

        self.slicer = gr.constellation_decoder_cb(rotated_const, range(arity))
        self.gray_decoder = gr.map_bb(gray_to_binary[arity])
        
        # unpack the k bit vector into a stream of bits
        self.unpack = gr.unpack_k_bits_bb(bits_per_symbol)

        fg.connect(self.agc, self.costas_loop, self.rrc_filter, self.clock_recovery,
                   self.slicer, self.gray_decoder, self.unpack)
        
        # Debug sinks
        if 1:
            fg.connect(self.agc,
                       gr.file_sink(gr.sizeof_gr_complex, "agc.dat"))
            fg.connect(self.costas_loop,
                       gr.file_sink(gr.sizeof_gr_complex, "costas_loop.dat"))
            fg.connect(self.rrc_filter,
                       gr.file_sink(gr.sizeof_gr_complex, "rrc.dat"))
            fg.connect(self.clock_recovery,
                       gr.file_sink(gr.sizeof_gr_complex, "clock_recovery.dat"))
            fg.connect(self.slicer,
                       gr.file_sink(gr.sizeof_char, "slicer.dat"))
            fg.connect(self.unpack,
                       gr.file_sink(gr.sizeof_char, "unpack.dat"))

        # Initialize base class
        gr.hier_block.__init__(self, fg, self.agc, self.unpack)

    def samples_per_baud(self):
        return self.spb

    def bits_per_baud(self):
        return self.bits_per_symbol


mpsk_demod = mpsk_demod__coherent_detection_of_differentially_encoded_psk
#mpsk_demod = mpsk_demod__coherent_detection_of_nondifferentially_encoded_psk
