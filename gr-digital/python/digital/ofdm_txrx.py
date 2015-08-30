#
# Copyright 2013 Free Software Foundation, Inc.
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
"""
OFDM Transmitter / Receiver hier blocks.

For simple configurations, no need to connect all the relevant OFDM blocks
to form an OFDM Tx/Rx--simply use these.
"""

# Reminder: All frequency-domain stuff is in shifted form, i.e. DC carrier
# in the middle!

import numpy
from gnuradio import gr
import digital_swig as digital
from utils import tagged_streams

try:
    # This will work when feature #505 is added.
    from gnuradio import fft
    from gnuradio import blocks
    from gnuradio import analog
except ImportError:
    # Until then this will work.
    import fft_swig as fft
    import blocks_swig as blocks
    import analog_swig as analog

_def_fft_len = 64
_def_cp_len = 16
_def_frame_length_tag_key = "frame_length"
_def_packet_length_tag_key = "packet_length"
_def_packet_num_tag_key = "packet_num"
# Data and pilot carriers are same as in 802.11a
_def_occupied_carriers = (range(-26, -21) + range(-20, -7) + range(-6, 0) + range(1, 7) + range(8, 21) + range(22, 27),)
_def_pilot_carriers=((-21, -7, 7, 21,),)
_pilot_sym_scramble_seq = (
        1,1,1,1, -1,-1,-1,1, -1,-1,-1,-1, 1,1,-1,1, -1,-1,1,1, -1,1,1,-1, 1,1,1,1, 1,1,-1,1,
        1,1,-1,1, 1,-1,-1,1, 1,1,-1,1, -1,-1,-1,1, -1,1,-1,-1, 1,-1,-1,1, 1,1,1,1, -1,-1,1,1,
        -1,-1,1,-1, 1,-1,1,1, -1,-1,-1,1, 1,-1,-1,-1, -1,1,-1,-1, 1,-1,1,1, 1,1,-1,1, -1,1,-1,1,
        -1,-1,-1,-1, -1,1,-1,1, 1,-1,1,-1, 1,1,1,-1, -1,1,-1,-1, -1,1,1,1, -1,-1,-1,-1, -1,-1,-1
)
_def_pilot_symbols= tuple([(x, x, x, -x) for x in _pilot_sym_scramble_seq])
_seq_seed = 42


def _get_active_carriers(fft_len, occupied_carriers, pilot_carriers):
    """ Returns a list of all carriers that at some point carry data or pilots. """
    active_carriers = list()
    for carrier in list(occupied_carriers[0]) + list(pilot_carriers[0]):
        if carrier < 0:
            carrier += fft_len
        active_carriers.append(carrier)
    return active_carriers

def _make_sync_word1(fft_len, occupied_carriers, pilot_carriers):
    """ Creates a random sync sequence for fine frequency offset and timing
    estimation. This is the first of typically two sync preamble symbols
    for the Schmidl & Cox sync algorithm.
    The relevant feature of this symbols is that every second sub-carrier
    is zero. In the time domain, this results in two identical halves of
    the OFDM symbols.
    Symbols are always BPSK symbols. Carriers are scaled by sqrt(2) to keep
    total energy constant.
    Carrier 0 (DC carrier) is always zero. If used, carrier 1 is non-zero.
    This means the sync algorithm has to check on odd carriers!
    """
    active_carriers = _get_active_carriers(fft_len, occupied_carriers, pilot_carriers)
    numpy.random.seed(_seq_seed)
    bpsk = {0: numpy.sqrt(2), 1: -numpy.sqrt(2)}
    sw1 = [bpsk[numpy.random.randint(2)]  if x in active_carriers and x % 2 else 0 for x in range(fft_len)]
    return numpy.fft.fftshift(sw1)

def _make_sync_word2(fft_len, occupied_carriers, pilot_carriers):
    """ Creates a random sync sequence for coarse frequency offset and channel
    estimation. This is the second of typically two sync preamble symbols
    for the Schmidl & Cox sync algorithm.
    Symbols are always BPSK symbols.
    """
    active_carriers = _get_active_carriers(fft_len, occupied_carriers, pilot_carriers)
    numpy.random.seed(_seq_seed)
    bpsk = {0: 1, 1: -1}
    sw2 = [bpsk[numpy.random.randint(2)] if x in active_carriers else 0 for x in range(fft_len)]
    sw2[0] = 0j
    return numpy.fft.fftshift(sw2)

def _get_constellation(bps):
    """ Returns a modulator block for a given number of bits per symbol """
    constellation = {
            1: digital.constellation_bpsk(),
            2: digital.constellation_qpsk(),
            3: digital.constellation_8psk()
    }
    try:
        return constellation[bps]
    except KeyError:
        print 'Modulation not supported.'
        exit(1)

class ofdm_tx(gr.hier_block2):
    """Hierarchical block for OFDM modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
    fft_len: The length of FFT (integer).
    cp_len:  The length of cyclic prefix in total samples (integer).
    packet_length_tag_key: The name of the tag giving packet length at the input.
    occupied_carriers: A vector of vectors describing which OFDM carriers are occupied.
    pilot_carriers: A vector of vectors describing which OFDM carriers are occupied with pilot symbols.
    pilot_symbols: The pilot symbols.
    bps_header: Bits per symbol (header).
    bps_payload: Bits per symbol (payload).
    sync_word1: The first sync preamble symbol. This has to be with
    |           zeros on alternating carriers. Used for fine and
    |           coarse frequency offset and timing estimation.
    sync_word2: The second sync preamble symbol. This has to be filled
    |           entirely. Also used for coarse frequency offset and
    |           channel estimation.
    rolloff: The rolloff length in samples. Must be smaller than the CP.
    debug_log: Write output into log files (Warning: creates lots of data!)
    scramble_bits: Activates the scramblers (set this to True unless debugging)

    """
    def __init__(self, fft_len=_def_fft_len, cp_len=_def_cp_len,
                 packet_length_tag_key=_def_packet_length_tag_key,
                 occupied_carriers=_def_occupied_carriers,
                 pilot_carriers=_def_pilot_carriers,
                 pilot_symbols=_def_pilot_symbols,
                 bps_header=1,
                 bps_payload=1,
                 sync_word1=None,
                 sync_word2=None,
                 rolloff=0,
                 debug_log=False,
                 scramble_bits=False
                 ):
        gr.hier_block2.__init__(self, "ofdm_tx",
                    gr.io_signature(1, 1, gr.sizeof_char),
                    gr.io_signature(1, 1, gr.sizeof_gr_complex))
        ### Param init / sanity check ########################################
        self.fft_len           = fft_len
        self.cp_len            = cp_len
        self.packet_length_tag_key = packet_length_tag_key
        self.occupied_carriers = occupied_carriers
        self.pilot_carriers    = pilot_carriers
        self.pilot_symbols     = pilot_symbols
        self.bps_header        = bps_header
        self.bps_payload       = bps_payload
        self.sync_word1 = sync_word1
        if sync_word1 is None:
            self.sync_word1 = _make_sync_word1(fft_len, occupied_carriers, pilot_carriers)
        else:
            if len(sync_word1) != self.fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
        self.sync_words = [self.sync_word1,]
        if sync_word2 is None:
            self.sync_word2 = _make_sync_word2(fft_len, occupied_carriers, pilot_carriers)
        else:
            self.sync_word2 = sync_word2
        if len(self.sync_word2):
            if len(self.sync_word2) != fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word2 = list(self.sync_word2)
            self.sync_words.append(self.sync_word2)
        if scramble_bits:
            self.scramble_seed = 0x7f
        else:
            self.scramble_seed = 0x00 # We deactivate the scrambler by init'ing it with zeros
        ### Header modulation ################################################
        crc = digital.crc32_bb(False, self.packet_length_tag_key)
        header_constellation  = _get_constellation(bps_header)
        header_mod = digital.chunks_to_symbols_bc(header_constellation.points())
        formatter_object = digital.packet_header_ofdm(
            occupied_carriers=occupied_carriers, n_syms=1,
            bits_per_header_sym=self.bps_header,
            bits_per_payload_sym=self.bps_payload,
            scramble_header=scramble_bits
        )
        header_gen = digital.packet_headergenerator_bb(formatter_object.base(), self.packet_length_tag_key)
        header_payload_mux = blocks.tagged_stream_mux(
                itemsize=gr.sizeof_gr_complex*1,
                lengthtagname=self.packet_length_tag_key,
                tag_preserve_head_pos=1 # Head tags on the payload stream stay on the head
        )
        self.connect(
                self,
                crc,
                header_gen,
                header_mod,
                (header_payload_mux, 0)
        )
        if debug_log:
            self.connect(header_gen, blocks.file_sink(1, 'tx-hdr.dat'))
        ### Payload modulation ###############################################
        payload_constellation = _get_constellation(bps_payload)
        payload_mod = digital.chunks_to_symbols_bc(payload_constellation.points())
        payload_scrambler = digital.additive_scrambler_bb(
            0x8a,
            self.scramble_seed,
            7,
            0, # Don't reset after fixed length (let the reset tag do that)
            bits_per_byte=8, # This is before unpacking
            reset_tag_key=self.packet_length_tag_key
        )
        payload_unpack = blocks.repack_bits_bb(
            8, # Unpack 8 bits per byte
            bps_payload,
            self.packet_length_tag_key
        )
        self.connect(
            crc,
            payload_scrambler,
            payload_unpack,
            payload_mod,
            (header_payload_mux, 1)
        )
        ### Create OFDM frame ################################################
        allocator = digital.ofdm_carrier_allocator_cvc(
            self.fft_len,
            occupied_carriers=self.occupied_carriers,
            pilot_carriers=self.pilot_carriers,
            pilot_symbols=self.pilot_symbols,
            sync_words=self.sync_words,
            len_tag_key=self.packet_length_tag_key
        )
        ffter = fft.fft_vcc(
                self.fft_len,
                False, # Inverse FFT
                (), # No window
                True # Shift
        )
        cyclic_prefixer = digital.ofdm_cyclic_prefixer(
            self.fft_len,
            self.fft_len+self.cp_len,
            rolloff,
            self.packet_length_tag_key
        )
        self.connect(header_payload_mux, allocator, ffter, cyclic_prefixer, self)
        if debug_log:
            self.connect(allocator,       blocks.file_sink(gr.sizeof_gr_complex * fft_len, 'tx-post-allocator.dat'))
            self.connect(cyclic_prefixer, blocks.file_sink(gr.sizeof_gr_complex,           'tx-signal.dat'))


class ofdm_rx(gr.hier_block2):
    """Hierarchical block for OFDM demodulation.

    The input is a complex baseband signal (e.g. from a UHD source).
    The detected packets are output as a stream of packed bits on the output.

    Args:
    fft_len: The length of FFT (integer).
    cp_len:  The length of cyclic prefix in total samples (integer).
    frame_length_tag_key: Used internally to tag the length of the OFDM frame.
    packet_length_tag_key: The name of the tag giving packet length at the input.
    occupied_carriers: A vector of vectors describing which OFDM carriers are occupied.
    pilot_carriers: A vector of vectors describing which OFDM carriers are occupied with pilot symbols.
    pilot_symbols: The pilot symbols.
    bps_header: Bits per symbol (header).
    bps_payload: Bits per symbol (payload).
    sync_word1: The first sync preamble symbol. This has to be with
    |           zeros on alternating carriers. Used for fine and
    |           coarse frequency offset and timing estimation.
    sync_word2: The second sync preamble symbol. This has to be filled
    |           entirely. Also used for coarse frequency offset and
    |           channel estimation.
    """
    def __init__(self, fft_len=_def_fft_len, cp_len=_def_cp_len,
                 frame_length_tag_key=_def_frame_length_tag_key,
                 packet_length_tag_key=_def_packet_length_tag_key,
                 packet_num_tag_key=_def_packet_num_tag_key,
                 occupied_carriers=_def_occupied_carriers,
                 pilot_carriers=_def_pilot_carriers,
                 pilot_symbols=_def_pilot_symbols,
                 bps_header=1,
                 bps_payload=1,
                 sync_word1=None,
                 sync_word2=None,
                 debug_log=False,
                 scramble_bits=False
                 ):
        gr.hier_block2.__init__(self, "ofdm_rx",
                    gr.io_signature(1, 1, gr.sizeof_gr_complex),
                    gr.io_signature(1, 1, gr.sizeof_char))
        ### Param init / sanity check ########################################
        self.fft_len           = fft_len
        self.cp_len            = cp_len
        self.frame_length_tag_key    = frame_length_tag_key
        self.packet_length_tag_key   = packet_length_tag_key
        self.occupied_carriers = occupied_carriers
        self.bps_header        = bps_header
        self.bps_payload       = bps_payload
        n_sync_words = 1
        if sync_word1 is None:
            self.sync_word1 = _make_sync_word1(fft_len, occupied_carriers, pilot_carriers)
        else:
            if len(sync_word1) != self.fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word1 = sync_word1
        self.sync_word2 = ()
        if sync_word2 is None:
            self.sync_word2 = _make_sync_word2(fft_len, occupied_carriers, pilot_carriers)
            n_sync_words = 2
        elif len(sync_word2):
            if len(sync_word2) != fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word2 = sync_word2
            n_sync_words = 2
        if scramble_bits:
            self.scramble_seed = 0x7f
        else:
            self.scramble_seed = 0x00 # We deactivate the scrambler by init'ing it with zeros
        ### Sync ############################################################
        sync_detect = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        delay = blocks.delay(gr.sizeof_gr_complex, fft_len+cp_len)
        oscillator = analog.frequency_modulator_fc(-2.0 / fft_len)
        mixer = blocks.multiply_cc()
        hpd = digital.header_payload_demux(
            n_sync_words+1,       # Number of OFDM symbols before payload (sync + 1 sym header)
            fft_len, cp_len,      # FFT length, guard interval
            frame_length_tag_key, # Frame length tag key
            "",                   # We're not using trigger tags
            True                  # One output item is one OFDM symbol (False would output complex scalars)
        )
        self.connect(self, sync_detect)
        self.connect(self, delay, (mixer, 0), (hpd, 0))
        self.connect((sync_detect, 0), oscillator, (mixer, 1))
        self.connect((sync_detect, 1), (hpd, 1))
        if debug_log:
            self.connect((sync_detect, 0), blocks.file_sink(gr.sizeof_float, 'freq-offset.dat'))
            self.connect((sync_detect, 1), blocks.file_sink(gr.sizeof_char, 'sync-detect.dat'))
        ### Header demodulation ##############################################
        header_fft           = fft.fft_vcc(self.fft_len, True, (), True)
        chanest              = digital.ofdm_chanest_vcvc(self.sync_word1, self.sync_word2, 1)
        header_constellation = _get_constellation(bps_header)
        header_equalizer     = digital.ofdm_equalizer_simpledfe(
            fft_len,
            header_constellation.base(),
            occupied_carriers,
            pilot_carriers,
            pilot_symbols,
            symbols_skipped=0,
        )
        header_eq = digital.ofdm_frame_equalizer_vcvc(
                header_equalizer.base(),
                cp_len,
                self.frame_length_tag_key,
                True,
                1 # Header is 1 symbol long
        )
        header_serializer = digital.ofdm_serializer_vcc(
                fft_len, occupied_carriers,
                self.frame_length_tag_key
        )
        header_demod     = digital.constellation_decoder_cb(header_constellation.base())
        header_formatter = digital.packet_header_ofdm(
                occupied_carriers, 1,
                packet_length_tag_key,
                frame_length_tag_key,
                packet_num_tag_key,
                bps_header,
                bps_payload,
                scramble_header=scramble_bits
        )
        header_parser = digital.packet_headerparser_b(header_formatter.formatter())
        self.connect(
                (hpd, 0),
                header_fft,
                chanest,
                header_eq,
                header_serializer,
                header_demod,
                header_parser
        )
        self.msg_connect(header_parser, "header_data", hpd, "header_data")
        if debug_log:
            self.connect((chanest, 1),      blocks.file_sink(gr.sizeof_gr_complex * fft_len, 'channel-estimate.dat'))
            self.connect((chanest, 0),      blocks.file_sink(gr.sizeof_gr_complex * fft_len, 'post-hdr-chanest.dat'))
            self.connect((chanest, 0),      blocks.tag_debug(gr.sizeof_gr_complex * fft_len, 'post-hdr-chanest'))
            self.connect(header_eq,         blocks.file_sink(gr.sizeof_gr_complex * fft_len, 'post-hdr-eq.dat'))
            self.connect(header_serializer, blocks.file_sink(gr.sizeof_gr_complex,           'post-hdr-serializer.dat'))
            self.connect(header_descrambler, blocks.file_sink(1,                             'post-hdr-demod.dat'))
        ### Payload demod ####################################################
        payload_fft = fft.fft_vcc(self.fft_len, True, (), True)
        payload_constellation = _get_constellation(bps_payload)
        payload_equalizer = digital.ofdm_equalizer_simpledfe(
                fft_len,
                payload_constellation.base(),
                occupied_carriers,
                pilot_carriers,
                pilot_symbols,
                symbols_skipped=1, # (that was already in the header)
                alpha=0.1
        )
        payload_eq = digital.ofdm_frame_equalizer_vcvc(
                payload_equalizer.base(),
                cp_len,
                self.frame_length_tag_key
        )
        payload_serializer = digital.ofdm_serializer_vcc(
                fft_len, occupied_carriers,
                self.frame_length_tag_key,
                self.packet_length_tag_key,
                1 # Skip 1 symbol (that was already in the header)
        )
        payload_demod = digital.constellation_decoder_cb(payload_constellation.base())
        self.payload_descrambler = digital.additive_scrambler_bb(
            0x8a,
            self.scramble_seed,
            7,
            0, # Don't reset after fixed length
            bits_per_byte=8, # This is after packing
            reset_tag_key=self.packet_length_tag_key
        )
        payload_pack = blocks.repack_bits_bb(bps_payload, 8, self.packet_length_tag_key, True)
        self.crc = digital.crc32_bb(True, self.packet_length_tag_key)
        self.connect(
                (hpd, 1),
                payload_fft,
                payload_eq,
                payload_serializer,
                payload_demod,
                payload_pack,
                self.payload_descrambler,
                self.crc,
                self
        )
        if debug_log:
            self.connect((hpd, 1),           blocks.tag_debug(gr.sizeof_gr_complex*fft_len, 'post-hpd'))
            self.connect(payload_fft,        blocks.file_sink(gr.sizeof_gr_complex*fft_len, 'post-payload-fft.dat'))
            self.connect(payload_eq,         blocks.file_sink(gr.sizeof_gr_complex*fft_len, 'post-payload-eq.dat'))
            self.connect(payload_serializer, blocks.file_sink(gr.sizeof_gr_complex,         'post-payload-serializer.dat'))
            self.connect(payload_demod,      blocks.file_sink(1,                            'post-payload-demod.dat'))
            self.connect(payload_pack,       blocks.file_sink(1,                            'post-payload-pack.dat'))
            self.connect(crc,                blocks.file_sink(1,                            'post-payload-crc.dat'))
