#!/usr/bin/env python
# Copyright 2012,2013 Free Software Foundation, Inc.
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

import numpy
from gnuradio import gr, gr_unittest
import pmt
import digital_swig as digital
import blocks_swig as blocks

class qa_ofdm_frame_equalizer_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_simple (self):
        """ Very simple functionality testing """
        fft_len = 8
        equalizer = digital.ofdm_equalizer_static(fft_len)
        n_syms = 3
        len_tag_key = "frame_len"
        tx_data = (1,) * fft_len * n_syms
        len_tag = gr.gr_tag_t()
        len_tag.offset = 0
        len_tag.key = pmt.string_to_symbol(len_tag_key)
        len_tag.value = pmt.from_long(n_syms)
        chan_tag = gr.gr_tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, (1,) * fft_len)
        src = blocks.vector_source_c(tx_data, False, fft_len, (len_tag, chan_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), len_tag_key)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, eq, sink)
        self.tb.run ()
        # Check data
        self.assertEqual(tx_data, sink.data())
        for tag in sink.tags():
            self.assertEqual(pmt.symbol_to_string(tag.key), len_tag_key)
            self.assertEqual(pmt.to_long(tag.value), n_syms)

    def test_002_static (self):
        fft_len = 8
        #           4   5  6  7   0  1  2   3
        tx_data = [-1, -1, 1, 2, -1, 3, 0, -1, # 0
                   -1, -1, 0, 2, -1, 2, 0, -1, # 8
                   -1, -1, 3, 0, -1, 1, 0, -1, # 16 (Pilot symbols)
                   -1, -1, 1, 1, -1, 0, 2, -1] # 24
        cnst = digital.constellation_qpsk()
        tx_signal = [cnst.map_to_points_v(x)[0] if x != -1 else 0 for x in tx_data]
        occupied_carriers = ((1, 2, 6, 7),)
        pilot_carriers = ((), (), (1, 2, 6, 7), ())
        pilot_symbols = (
                [], [], [cnst.map_to_points_v(x)[0] for x in (1, 0, 3, 0)], []
        )
        equalizer = digital.ofdm_equalizer_static(fft_len, occupied_carriers, pilot_carriers, pilot_symbols)
        channel = [
            0, 0,  1,  1, 0,  1,  1, 0,
            0, 0,  1,  1, 0,  1,  1, 0, # These coefficients will be rotated slightly...
            0, 0, 1j, 1j, 0, 1j, 1j, 0, # Go crazy here!
            0, 0, 1j, 1j, 0, 1j, 1j, 0  # ...and again here.
        ]
        for idx in range(fft_len, 2*fft_len):
            channel[idx] = channel[idx-fft_len] * numpy.exp(1j * .1 * numpy.pi * (numpy.random.rand()-.5))
            idx2 = idx+2*fft_len
            channel[idx2] = channel[idx2] * numpy.exp(1j * 0 * numpy.pi * (numpy.random.rand()-.5))
        len_tag_key = "frame_len"
        len_tag = gr.gr_tag_t()
        len_tag.offset = 0
        len_tag.key = pmt.string_to_symbol(len_tag_key)
        len_tag.value = pmt.from_long(4)
        chan_tag = gr.gr_tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, channel[:fft_len])
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len, (len_tag, chan_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), len_tag_key, True)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, eq, sink)
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()]
        self.assertEqual(tx_data, rx_data)
        for tag in sink.tags():
            if pmt.symbol_to_string(tag.key) == len_tag_key:
                self.assertEqual(pmt.to_long(tag.value), 4)
            if pmt.symbol_to_string(tag.key) == "ofdm_sync_chan_taps":
                self.assertEqual(list(pmt.c32vector_elements(tag.value)), channel[-fft_len:])

    def test_002_simpledfe (self):
        fft_len = 8
        #           4   5  6  7   0  1  2   3
        tx_data = [-1, -1, 1, 2, -1, 3, 0, -1, # 0
                   -1, -1, 0, 2, -1, 2, 0, -1, # 8
                   -1, -1, 3, 0, -1, 1, 0, -1, # 16 (Pilot symbols)
                   -1, -1, 1, 1, -1, 0, 2, -1] # 24
        cnst = digital.constellation_qpsk()
        tx_signal = [cnst.map_to_points_v(x)[0] if x != -1 else 0 for x in tx_data]
        occupied_carriers = ((1, 2, 6, 7),)
        pilot_carriers = ((), (), (1, 2, 6, 7), ())
        pilot_symbols = (
                [], [], [cnst.map_to_points_v(x)[0] for x in (1, 0, 3, 0)], []
        )
        equalizer = digital.ofdm_equalizer_simpledfe(
            fft_len, cnst.base(), occupied_carriers, pilot_carriers, pilot_symbols, 0, 0.01
        )
        channel = [
            0, 0,  1,  1, 0,  1,  1, 0,
            0, 0,  1,  1, 0,  1,  1, 0, # These coefficients will be rotated slightly...
            0, 0, 1j, 1j, 0, 1j, 1j, 0, # Go crazy here!
            0, 0, 1j, 1j, 0, 1j, 1j, 0  # ...and again here.
        ]
        for idx in range(fft_len, 2*fft_len):
            channel[idx] = channel[idx-fft_len] * numpy.exp(1j * .1 * numpy.pi * (numpy.random.rand()-.5))
            idx2 = idx+2*fft_len
            channel[idx2] = channel[idx2] * numpy.exp(1j * 0 * numpy.pi * (numpy.random.rand()-.5))
        len_tag_key = "frame_len"
        len_tag = gr.gr_tag_t()
        len_tag.offset = 0
        len_tag.key = pmt.string_to_symbol(len_tag_key)
        len_tag.value = pmt.from_long(4)
        chan_tag = gr.gr_tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, channel[:fft_len])
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len, (len_tag, chan_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), len_tag_key, True)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, eq, sink)
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()]
        self.assertEqual(tx_data, rx_data)
        for tag in sink.tags():
            if pmt.symbol_to_string(tag.key) == len_tag_key:
                self.assertEqual(pmt.to_long(tag.value), 4)
            if pmt.symbol_to_string(tag.key) == "ofdm_sync_chan_taps":
                self.assertComplexTuplesAlmostEqual(list(pmt.c32vector_elements(tag.value)), channel[-fft_len:], places=1)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_frame_equalizer_vcvc, "qa_ofdm_frame_equalizer_vcvc.xml")

