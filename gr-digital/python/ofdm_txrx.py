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
"""
OFDM Transmitter / Receiver hier blocks.

For simple configurations, no need to connect all the relevant OFDM blocks
to form an OFDM Tx/Rx--simply use these.
"""

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
_def_packet_length_tag_key = "frame_length"
_def_packet_num_tag_key = ""
_def_occupied_carriers=(range(1, 27) + range(38, 64),)
_def_pilot_carriers=((0,),)
_def_pilot_symbols=((100,),)
_seq_seed = 42

def _make_sync_word(fft_len, occupied_carriers, constellation):
    """ Makes a random sync sequence """
    occupied_carriers = list(occupied_carriers[0])
    occupied_carriers = [occupied_carriers[x] + fft_len if occupied_carriers[x] < 0 else occupied_carriers[x] for x in range(len(occupied_carriers))]
    numpy.random.seed(_seq_seed)
    sync_sequence = [constellation.map_to_points_v(numpy.random.randint(constellation.arity()))[0] * numpy.sqrt(2) if x in occupied_carriers and x % 3 else 0 for x in range(fft_len)]
    return sync_sequence

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
    """
    Hierarchical block for OFDM modulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
    fft_len: The length of FFT (integer).
    cp_len: The length of cyclic prefix (integer).
    occupied_carriers: ??
    pilot_carriers: ??
    pilot_symbols: ?? 
    length_tag_key: The name of the tag giving packet length.
    """
    def __init__(self, fft_len=_def_fft_len, cp_len=_def_cp_len,
                 frame_length_tag_key=_def_frame_length_tag_key,
                 occupied_carriers=_def_occupied_carriers,
                 pilot_carriers=_def_pilot_carriers,
                 pilot_symbols=_def_pilot_symbols,
                 bps_header=1,
                 bps_payload=1,
                 sync_word1=None,
                 sync_word2=None,
                 rolloff=0
                 ):
        gr.hier_block2.__init__(self, "ofdm_tx",
                    gr.io_signature(1, 1, gr.sizeof_char),
                    gr.io_signature(1, 1, gr.sizeof_gr_complex))
        self.fft_len           = fft_len
        self.cp_len            = cp_len
        self.frame_length_tag_key    = frame_length_tag_key
        self.occupied_carriers = occupied_carriers
        self.pilot_carriers    = pilot_carriers
        self.pilot_symbols     = pilot_symbols
        self.bps_header        = bps_header
        self.bps_payload       = bps_payload
        n_sync_words = 1
        header_constellation  = _get_constellation(bps_header)
        header_mod = digital.chunks_to_symbols_bc(header_constellation.points())
        self.sync_word1 = sync_word1
        if sync_word1 is None:
            self.sync_word1 = _make_sync_word(fft_len, occupied_carriers, header_constellation)
        else:
            if len(sync_word1) != self.fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
        total_sync_word = self.sync_word1
        self.sync_word2 = ()
        if sync_word2 is not None:
            if len(sync_word2) != fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word2 = sync_word2
            n_sync_words = 2
            total_sync_word = sync_word1 + sync_word2
        crc = digital.crc32_bb(False, self.frame_length_tag_key)
        formatter_object = digital.packet_header_ofdm(
            occupied_carriers=occupied_carriers, n_syms=1,
            bits_per_sym=self.bps_header
        )
        header_gen = digital.packet_headergenerator_bb(formatter_object.base(), self.frame_length_tag_key)
        header_payload_mux = blocks.tagged_stream_mux(gr.sizeof_gr_complex*1, self.frame_length_tag_key)
        self.connect(self, crc, header_gen, header_mod, (header_payload_mux, 0))
        payload_constellation = _get_constellation(bps_payload)
        payload_mod = digital.chunks_to_symbols_bc(payload_constellation.points())
        self.connect(
            crc,
            blocks.repack_bits_bb(8, bps_payload, frame_length_tag_key),
            payload_mod,
            (header_payload_mux, 1)
        )
        self.connect(payload_mod, gr.tag_debug(gr.sizeof_gr_complex, "pmod"))
        sync_word_gen = gr.vector_source_c(
            total_sync_word, True, self.fft_len,
            tagged_streams.make_lengthtags((n_sync_words,), (0,), self.frame_length_tag_key)
        )
        allocator = digital.ofdm_carrier_allocator_cvc(
            self.fft_len,
            occupied_carriers=self.occupied_carriers,
            pilot_carriers=self.pilot_carriers,
            pilot_symbols=self.pilot_symbols,
            len_tag_key=self.frame_length_tag_key
        )
        syncword_data_mux  = blocks.tagged_stream_mux(gr.sizeof_gr_complex*self.fft_len, self.frame_length_tag_key)
        self.connect(sync_word_gen, (syncword_data_mux, 0))
        self.connect(header_payload_mux, allocator, (syncword_data_mux, 1))
        ffter = fft.fft_vcc(self.fft_len, False, (), False)
        cyclic_prefixer = digital.ofdm_cyclic_prefixer(
            self.fft_len,
            self.fft_len+self.cp_len,
            rolloff,
            self.frame_length_tag_key
        )
        self.connect(syncword_data_mux, ffter, cyclic_prefixer, self)


class ofdm_rx(gr.hier_block2):
    """
    Hierarchical block for OFDM demodulation.

    The input is a byte stream (unsigned char) and the
    output is the complex modulated signal at baseband.

    Args:
    fft_len: The length of FFT (integer).
    cp_len: The length of cyclic prefix (integer).
    occupied_carriers: ??
    pilot_carriers: ??
    pilot_symbols: ?? 
    length_tag_key: The name of the tag giving packet length.
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
                 sync_word2=None
                 ):
        gr.hier_block2.__init__(self, "ofdm_rx",
                    gr.io_signature(1, 1, gr.sizeof_gr_complex),
                    gr.io_signature(1, 1, gr.sizeof_char))
        self.fft_len           = fft_len
        self.cp_len            = cp_len
        self.frame_length_tag_key    = frame_length_tag_key
        self.packet_length_tag_key   = packet_length_tag_key
        self.occupied_carriers = occupied_carriers
        self.bps_header        = bps_header
        self.bps_payload       = bps_payload
        n_sync_words = 1
        header_constellation  = _get_constellation(bps_header)
        if sync_word1 is None:
            self.sync_word1 = _make_sync_word(fft_len, occupied_carriers, header_constellation)
        else:
            if len(sync_word1) != self.fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word1 = sync_word1
        self.sync_word2 = ()
        if sync_word2 is not None:
            if len(sync_word2) != fft_len:
                raise ValueError("Length of sync sequence(s) must be FFT length.")
            self.sync_word2 = sync_word2
            n_sync_words = 2
        else:
            sync_word2 = ()
        # Receiver path
        sync_detect = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        oscillator = analog.frequency_modulator_fc(-2.0 / fft_len)
        delay = gr.delay(gr.sizeof_gr_complex, fft_len+cp_len)
        mixer = gr.multiply_cc()
        hpd = digital.header_payload_demux(n_sync_words, fft_len, cp_len,
                frame_length_tag_key, "", True)
        self.connect(self, sync_detect)
        self.connect((sync_detect, 0), oscillator, (mixer, 0))
        self.connect(self, delay, (mixer, 1))
        self.connect(mixer, (hpd, 0))
        self.connect((sync_detect, 1), (hpd, 1))
        # Header demodulation
        header_fft = fft.fft_vcc(self.fft_len, True, (), True)
        chanest = digital.ofdm_chanest_vcvc(self.sync_word1, self.sync_word2, 1)
        header_equalizer = digital.ofdm_equalizer_simpledfe(
                fft_len, header_constellation.base(),
                occupied_carriers, pilot_carriers, pilot_symbols
        )
        header_eq = digital.ofdm_frame_equalizer_vcvc(header_equalizer.base(), frame_length_tag_key, True)
        header_serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers)
        header_constellation = _get_constellation(bps_header)
        header_demod = digital.constellation_decoder_cb(header_constellation.base())
        header_formatter = digital.packet_header_ofdm(
                occupied_carriers, 1,
                packet_length_tag_key,
                frame_length_tag_key,
                packet_num_tag_key,
                bps_header
        )
        header_parser = digital.packet_headerparser_b(header_formatter.formatter())
        self.connect((hpd, 0), header_fft, chanest, header_eq, header_serializer, header_demod, header_parser)
        self.msg_connect(header_parser, "header_data", hpd, "header_data")
        # Payload demodulation
        payload_fft = fft.fft_vcc(self.fft_len, True, (), True)
        payload_equalizer = digital.ofdm_equalizer_simpledfe(
                fft_len, header_constellation.base(),
                occupied_carriers, pilot_carriers, pilot_symbols, 1
        )
        payload_eq = digital.ofdm_frame_equalizer_vcvc(payload_equalizer.base(), frame_length_tag_key)
        payload_serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers)
        payload_constellation = _get_constellation(bps_payload)
        payload_demod = digital.constellation_decoder_cb(payload_constellation.base())
        bit_packer = blocks.repack_bits_bb(bps_payload, 8, packet_length_tag_key, True)
        self.connect((hpd, 1), payload_fft, payload_eq, payload_serializer, payload_demod, bit_packer, self)

