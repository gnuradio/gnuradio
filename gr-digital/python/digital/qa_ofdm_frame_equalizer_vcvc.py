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

from gnuradio import gr, gr_unittest, digital, blocks
import pmt

class qa_ofdm_frame_equalizer_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "tsb_key"

    def tearDown (self):
        self.tb = None

    def test_001_simple (self):
        """ Very simple functionality testing:
        - static equalizer
        - init channel state with all ones
        - transmit all ones
        - make sure we rx all ones
        - Tag check: put in frame length tag and one other random tag,
                     make sure they're propagated
        """
        fft_len = 8
        equalizer = digital.ofdm_equalizer_static(fft_len)
        n_syms = 3
        tx_data = (1,) * fft_len * n_syms
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, (1,) * fft_len)
        random_tag = gr.tag_t()
        random_tag.offset = 1
        random_tag.key = pmt.string_to_symbol("foo")
        random_tag.value = pmt.from_long(42)
        src = blocks.vector_source_c(tx_data, False, fft_len, (chan_tag, random_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, self.tsb_key)
        sink = blocks.tsb_vector_sink_c(fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key),
            eq,
            sink
        )
        self.tb.run ()
        # Check data
        self.assertEqual(tx_data, sink.data()[0])
        # Check tags
        tag_dict = dict()
        for tag in sink.tags():
            ptag = gr.tag_to_python(tag)
            tag_dict[ptag.key] = ptag.value
        expected_dict = {
            'foo': 42
        }
        self.assertEqual(tag_dict, expected_dict)

    def test_001b_simple_skip_nothing (self):
        """
        Same as before, but put a skip-header in there
        """
        fft_len = 8
        equalizer = digital.ofdm_equalizer_static(fft_len, symbols_skipped=1)
        n_syms = 3
        tx_data = (1,) * fft_len * n_syms
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, (1,) * fft_len)
        src = blocks.vector_source_c(tx_data, False, fft_len, (chan_tag,))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, self.tsb_key)
        sink = blocks.tsb_vector_sink_c(fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key),
            eq,
            sink
        )
        self.tb.run ()
        # Check data
        self.assertEqual(tx_data, sink.data()[0])

    def test_001c_carrier_offset_no_cp (self):
        """
        Same as before, but put a carrier offset in there
        """
        fft_len = 8
        cp_len = 0
        n_syms = 1
        carr_offset = 1
        occupied_carriers = ((-2, -1, 1, 2),)
        tx_data = (
                0, 0, 0,  -1j,  -1j, 0, -1j, -1j,
        )
        # The rx'd signal is shifted
        rx_expected = (0, 0, 1, 1, 0, 1, 1, 0) * n_syms
        equalizer = digital.ofdm_equalizer_static(fft_len, occupied_carriers)
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        # Note: this is shifted to the correct position!
        chan_tag.value = pmt.init_c32vector(fft_len, (0, 0, -1j, -1j, 0, -1j, -1j, 0))
        offset_tag = gr.tag_t()
        offset_tag.offset = 0
        offset_tag.key = pmt.string_to_symbol("ofdm_sync_carr_offset")
        offset_tag.value = pmt.from_long(carr_offset)
        src = blocks.vector_source_c(tx_data, False, fft_len, (chan_tag, offset_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), cp_len, self.tsb_key)
        sink = blocks.tsb_vector_sink_c(fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key),
            eq,
            sink
        )
        self.tb.run ()
        # Check data
        self.assertComplexTuplesAlmostEqual(rx_expected, sink.data()[0], places=4)

    def test_001c_carrier_offset_cp (self):
        """
        Same as before, but put a carrier offset in there and a CP
        """
        fft_len = 8
        cp_len = 2
        n_syms = 3
        # cp_len/fft_len == 1/4, therefore, the phase is rotated by
        # carr_offset * \pi/2 in every symbol
        occupied_carriers = ((-2, -1, 1, 2),)
        carr_offset = -1
        tx_data = (
                0,-1j,-1j, 0,-1j,-1j, 0, 0,
                0, -1, -1, 0, -1, -1, 0, 0,
                0, 1j, 1j, 0, 1j, 1j, 0, 0,
        )
        # Rx'd signal is corrected
        rx_expected = (0, 0, 1, 1, 0, 1, 1, 0) * n_syms
        equalizer = digital.ofdm_equalizer_static(fft_len, occupied_carriers)
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, (0, 0, 1, 1, 0, 1, 1, 0))
        offset_tag = gr.tag_t()
        offset_tag.offset = 0
        offset_tag.key = pmt.string_to_symbol("ofdm_sync_carr_offset")
        offset_tag.value = pmt.from_long(carr_offset)
        src = blocks.vector_source_c(tx_data, False, fft_len, (chan_tag, offset_tag))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), cp_len, self.tsb_key)
        sink = blocks.tsb_vector_sink_c(fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key),
            eq,
            sink
        )
        self.tb.run ()
        # Check data
        self.assertComplexTuplesAlmostEqual(rx_expected, sink.data()[0], places=4)

    def test_002_static (self):
        """
        - Add a simple channel
        - Make symbols QPSK
        """
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
            0, 0,  1,  1, 0,  1,  1, 0, # These coefficients will be rotated slightly (but less than \pi/2)
            0, 0, 1j, 1j, 0, 1j, 1j, 0, # Go crazy here!
            0, 0, 1j, 1j, 0, 1j, 1j, 0
        ]
        channel = [
            0, 0,  1,  1, 0,  1,  1, 0,
            0, 0,  1,  1, 0,  1,  1, 0, # These coefficients will be rotated slightly (but less than \pi/2)
            0, 0, 1j, 1j, 0, 1j, 1j, 0, # Go crazy here!
            0, 0, 1j, 1j, 0, 1j, 1j, 0
        ]
        for idx in range(fft_len, 2*fft_len):
            channel[idx] = channel[idx-fft_len] * numpy.exp(1j * .1 * numpy.pi * (numpy.random.rand()-.5))
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, channel[:fft_len])
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len, (chan_tag,))
        sink = blocks.tsb_vector_sink_c(vlen=fft_len, tsb_key=self.tsb_key)
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, self.tsb_key, True)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, len(tx_data)/fft_len, self.tsb_key),
                eq,
                sink
        )
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()[0]]
        # Check data
        self.assertEqual(tx_data, rx_data)
        # Check tags
        tag_dict = dict()
        for tag in sink.tags():
            ptag = gr.tag_to_python(tag)
            tag_dict[ptag.key] = ptag.value
            if ptag.key == 'ofdm_sync_chan_taps':
                tag_dict[ptag.key] = list(pmt.c32vector_elements(tag.value))
            else:
                tag_dict[ptag.key] = pmt.to_python(tag.value)
        expected_dict = {
                'ofdm_sync_chan_taps': channel[-fft_len:]
        }
        self.assertEqual(tag_dict, expected_dict)

    def test_002_static_wo_tags (self):
        """ Same as before, but the input stream has no tag.
        We specify the frame size in the constructor.
        We also specify a tag key, so the output stream *should* have
        a TSB tag.
        """
        fft_len = 8
        n_syms = 4
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
            0, 0,  1,  1, 0,  1,  1, 0, # These coefficients will be rotated slightly (below)...
            0, 0, 1j, 1j, 0, 1j, 1j, 0, # Go crazy here!
            0, 0, 1j, 1j, 0, 1j, 1j, 0  # ...and again here.
        ]
        for idx in range(fft_len, 2*fft_len):
            channel[idx] = channel[idx-fft_len] * numpy.exp(1j * .1 * numpy.pi * (numpy.random.rand()-.5))
            idx2 = idx+2*fft_len
            channel[idx2] = channel[idx2] * numpy.exp(1j * 0 * numpy.pi * (numpy.random.rand()-.5))
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len)
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, self.tsb_key, False, n_syms)
        sink = blocks.tsb_vector_sink_c(vlen=fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, len(tx_data)/fft_len, self.tsb_key),
                eq,
                sink
        )
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()[0]]
        self.assertEqual(tx_data, rx_data)
        # Check TSB Functionality
        packets = sink.data()
        self.assertEqual(len(packets), 1)
        self.assertEqual(len(packets[0]), len(tx_data))

    def test_002_static_wo_tags (self):
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
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len)
        sink = blocks.vector_sink_c(fft_len)
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, "", False, 4)
        self.tb.connect(src, eq, sink)
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()]
        self.assertEqual(tx_data, rx_data)

    def test_002_simpledfe (self):
        """ Use the simple DFE equalizer. """
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
        chan_tag = gr.tag_t()
        chan_tag.offset = 0
        chan_tag.key = pmt.string_to_symbol("ofdm_sync_chan_taps")
        chan_tag.value = pmt.init_c32vector(fft_len, channel[:fft_len])
        src = blocks.vector_source_c(numpy.multiply(tx_signal, channel), False, fft_len, (chan_tag,))
        eq = digital.ofdm_frame_equalizer_vcvc(equalizer.base(), 0, self.tsb_key, True)
        sink = blocks.tsb_vector_sink_c(fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, len(tx_data)/fft_len, self.tsb_key),
                eq,
                sink
        )
        self.tb.run ()
        rx_data = [cnst.decision_maker_v((x,)) if x != 0 else -1 for x in sink.data()[0]]
        self.assertEqual(tx_data, rx_data)
        self.assertEqual(len(sink.tags()), 1)
        tag = sink.tags()[0]
        self.assertEqual(pmt.symbol_to_string(tag.key), "ofdm_sync_chan_taps")
        self.assertComplexTuplesAlmostEqual(list(pmt.c32vector_elements(tag.value)), channel[-fft_len:], places=1)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_frame_equalizer_vcvc, "qa_ofdm_frame_equalizer_vcvc.xml")

