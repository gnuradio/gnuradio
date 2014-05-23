#!/usr/bin/env python
#
# Copyright 2012-2014 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks, fft, analog, digital
import pmt

class qa_ofdm_serializer_vcc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "ts_last"

    def tearDown (self):
        self.tb = None

    def test_001_simple (self):
        """ Standard test """
        fft_len = 16
        tx_symbols = (0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,  0,
                      0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12, 0,
                      0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0,  0)
        expected_result = tuple(range(1, 16)) + (0, 0, 0)
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        n_syms = len(tx_symbols)/fft_len
        src = blocks.vector_source_c(tx_symbols, False, fft_len)
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, self.tsb_key, "", 0, "", False)
        sink = blocks.tsb_vector_sink_c(tsb_key=self.tsb_key)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key), serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)

    def test_001b_shifted (self):
        """ Same as before, but shifted, because that's the normal mode in OFDM Rx """
        fft_len = 16
        tx_symbols = (
            0, 0, 0, 0, 0,  0,  1,  2,    0,   3, 4,   5,  0, 0, 0, 0,
            0, 0, 0, 0, 6, 1j,  7,  8,    0,   9, 10, 1j, 11, 0, 0, 0,
            0, 0, 0, 0, 0, 12, 13, 14,    0,  15, 16, 17,  0, 0, 0, 0,
        )
        expected_result = tuple(range(18))
        occupied_carriers = ((13, 14, 15, 1, 2, 3), (-4, -2, -1, 1, 2, 4),)
        n_syms = len(tx_symbols)/fft_len
        src = blocks.vector_source_c(tx_symbols, False, fft_len)
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, self.tsb_key)
        sink = blocks.tsb_vector_sink_c(tsb_key=self.tsb_key)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key), serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)

    def test_002_with_offset (self):
        """ Standard test, carrier offset """
        fft_len = 16
        tx_symbols = range(1, 16);
        tx_symbols = (0, 0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,
                      0, 0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12,
                      0, 0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0)
        carr_offset = 1 # Compare this with tx_symbols from the previous test
        expected_result = tuple(range(1, 16)) + (0, 0, 0)
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        n_syms = len(tx_symbols)/fft_len
        offsettag = gr.tag_t()
        offsettag.offset = 0
        offsettag.key = pmt.string_to_symbol("ofdm_sync_carr_offset")
        offsettag.value = pmt.from_long(carr_offset)
        src = blocks.vector_source_c(tx_symbols, False, fft_len, (offsettag,))
        sink = blocks.tsb_vector_sink_c(tsb_key=self.tsb_key)
        serializer = digital.ofdm_serializer_vcc(
                fft_len,
                occupied_carriers,
                self.tsb_key,
                "", 0,
                "ofdm_sync_carr_offset",
                False
        )
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key), serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)
        self.assertEqual(len(sink.tags()), 1)

    def test_003_connect (self):
        """ Connect carrier_allocator to ofdm_serializer,
            make sure output==input """
        fft_len = 8
        n_syms = 1
        occupied_carriers = ((1, 2, 6, 7),)
        pilot_carriers = ((3,),(5,))
        pilot_symbols = ((1j,),(-1j,))
        #tx_data = tuple([numpy.random.randint(0, 10) for x in range(4 * n_syms)])
        tx_data = (1, 2, 3, 4)
        src = blocks.vector_source_c(tx_data, False, 1)
        alloc = digital.ofdm_carrier_allocator_cvc(
                fft_len,
                occupied_carriers,
                pilot_carriers,
                pilot_symbols,
                (), # No sync word
                self.tsb_key,
                True # Output is shifted (default)
        )
        serializer = digital.ofdm_serializer_vcc(
                alloc,
                "", # Len tag key
                0, # Symbols skipped
                "", # Carrier offset key
                True # Input is shifted (default)
        )
        sink = blocks.tsb_vector_sink_c(tsb_key=self.tsb_key)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_data), self.tsb_key), alloc, serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], tx_data)

    def test_004_connect (self):
        """
        Advanced test:
        - Allocator -> IFFT -> Frequency offset -> FFT -> Serializer
        - FFT does shift (moves DC to middle)
        - Make sure input == output
        - Frequency offset is -2 carriers
        """
        fft_len = 8
        n_syms = 1
        carr_offset = -2
        freq_offset = 1.0 / fft_len * carr_offset # Normalized frequency
        occupied_carriers = ((-2, -1, 1, 2),)
        pilot_carriers = ((-3,),(3,))
        pilot_symbols = ((1j,),(-1j,))
        tx_data = (1, 2, 3, 4)
        offsettag = gr.tag_t()
        offsettag.offset = 0
        offsettag.key = pmt.string_to_symbol("ofdm_sync_carr_offset")
        offsettag.value = pmt.from_long(carr_offset)
        src = blocks.vector_source_c(tx_data, False, 1, (offsettag,))
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols, (),
                       self.tsb_key)
        tx_ifft = fft.fft_vcc(fft_len, False, (1.0/fft_len,)*fft_len, True)
        oscillator = analog.sig_source_c(1.0, analog.GR_COS_WAVE, freq_offset, 1.0)
        mixer = blocks.multiply_cc()
        rx_fft  = fft.fft_vcc(fft_len, True, (), True)
        sink2 = blocks.tsb_vector_sink_c(vlen=fft_len, tsb_key=self.tsb_key)
        self.tb.connect(rx_fft, sink2)
        serializer = digital.ofdm_serializer_vcc(
                alloc, "", 0, "ofdm_sync_carr_offset", True
        )
        sink = blocks.tsb_vector_sink_c(tsb_key=self.tsb_key)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_data), self.tsb_key),
                alloc, tx_ifft,
                blocks.vector_to_stream(gr.sizeof_gr_complex, fft_len),
                (mixer, 0),
                blocks.stream_to_vector(gr.sizeof_gr_complex, fft_len),
                rx_fft, serializer, sink
        )
        self.tb.connect(oscillator, (mixer, 1))
        self.tb.run ()
        self.assertComplexTuplesAlmostEqual(sink.data()[0][-len(occupied_carriers[0]):], tx_data, places=4)

    def test_005_packet_len_tag (self):
        """ Standard test """
        fft_len = 16
        tx_symbols = range(1, 16);
        tx_symbols = (0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,  0,
                      0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12, 0,
                      0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0,  0)
        expected_result = tuple(range(1, 16))
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        n_syms = len(tx_symbols)/fft_len
        packet_len_tsb_key = "packet_len"
        tag2 = gr.tag_t()
        tag2.offset = 0
        tag2.key = pmt.string_to_symbol("packet_len")
        tag2.value = pmt.from_long(len(expected_result))
        src = blocks.vector_source_c(tx_symbols, False, fft_len, (tag2,))
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, self.tsb_key, packet_len_tsb_key , 0, "", False)
        sink = blocks.tsb_vector_sink_c(tsb_key=packet_len_tsb_key)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, n_syms, self.tsb_key), serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)

    def test_099 (self):
        """ Make sure it fails if it should """
        fft_len = 16
        occupied_carriers = ((1, 3, 4, 11, 12, 112),) # Something invalid
        self.assertRaises(RuntimeError, digital.ofdm_serializer_vcc, fft_len, occupied_carriers, self.tsb_key)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_serializer_vcc, "qa_ofdm_serializer_vcc.xml")

