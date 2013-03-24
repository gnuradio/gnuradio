#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import fft_swig as fft
import digital_swig as digital
try: import pmt
except: from gruel import pmt
import numpy

class qa_ofdm_serializer_vcc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_simple (self):
        """ Standard test """
        fft_len = 16
        tx_symbols = range(1, 16);
        tx_symbols = (0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,  0,
                      0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12, 0,
                      0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0,  0)
        expected_result = tuple(range(1, 16)) + (0, 0, 0)
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        n_syms = len(tx_symbols)/fft_len
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(n_syms)
        src = gr.vector_source_c(tx_symbols, False, fft_len, (tag,))
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, tag_name, "", 0, False)
        sink = gr.vector_sink_c()
        self.tb.connect(src, serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_result)
        self.assertEqual(len(sink.tags()), 1)
        result_tag = sink.tags()[0]
        self.assertEqual(pmt.pmt_symbol_to_string(result_tag.key), tag_name)
        self.assertEqual(pmt.pmt_to_long(result_tag.value), n_syms * len(occupied_carriers[0]))

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
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(n_syms)
        offsettag = gr.gr_tag_t()
        offsettag.offset = 0
        offsettag.key = pmt.pmt_string_to_symbol("ofdm_sync_carr_offset")
        offsettag.value = pmt.pmt_from_long(carr_offset)
        src = gr.vector_source_c(tx_symbols, False, fft_len, (tag, offsettag))
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, tag_name, "", 0, False)
        sink = gr.vector_sink_c()
        self.tb.connect(src, serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_result)
        self.assertEqual(len(sink.tags()), 2)
        for tag in sink.tags():
            if pmt.pmt_symbol_to_string(tag.key) == tag_name:
                self.assertEqual(pmt.pmt_to_long(tag.value), n_syms * len(occupied_carriers[0]))

    def test_003_connect (self):
        """ Connect carrier_allocator to ofdm_serializer,
            make sure output==input """
        fft_len = 8
        n_syms = 10
        occupied_carriers = ((1, 2, 6, 7),)
        pilot_carriers = ((3,),(5,))
        pilot_symbols = ((1j,),(-1j,))
        tx_data = tuple([numpy.random.randint(0, 10) for x in range(4 * n_syms)])
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(len(tx_data))
        src = gr.vector_source_c(tx_data, False, 1, (tag,))
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       tag_name)
        serializer = digital.ofdm_serializer_vcc(alloc)
        sink = gr.vector_sink_c()
        self.tb.connect(src, alloc, serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), tx_data)

    def test_004_connect (self):
        """
        Advanced test:
        - Allocator -> IFFT -> Frequency offset -> FFT -> Serializer
        - FFT does shift (moves DC to middle)
        - Make sure input == output
        - Frequency offset is -2 carriers
        """
        fft_len = 8
        n_syms = 2
        carr_offset = -2
        freq_offset = 2 * numpy.pi * carr_offset / fft_len # If the sampling rate == 1
        occupied_carriers = ((1, 2, -2, -1),)
        pilot_carriers = ((3,),(5,))
        pilot_symbols = ((1j,),(-1j,))
        tx_data = tuple([numpy.random.randint(0, 10) for x in range(4 * n_syms)])
        #tx_data = (1,) * occupied_carriers[0] * n_syms
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(len(tx_data))
        offsettag = gr.gr_tag_t()
        offsettag.offset = 0
        offsettag.key = pmt.pmt_string_to_symbol("ofdm_sync_carr_offset")
        offsettag.value = pmt.pmt_from_long(carr_offset)
        src = gr.vector_source_c(tx_data, False, 1, (tag, offsettag))
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       tag_name)
        tx_ifft = fft.fft_vcc(fft_len, False, ())
        offset_sig = gr.sig_source_c(1.0, gr.GR_COS_WAVE, freq_offset, 1.0)
        mixer = gr.multiply_cc()
        rx_fft  = fft.fft_vcc(fft_len, True, (), True)
        serializer = digital.ofdm_serializer_vcc(alloc)
        sink = gr.vector_sink_c()
        self.tb.connect(
                src, alloc, tx_ifft,
                gr.vector_to_stream(gr.sizeof_gr_complex, fft_len),
                (mixer, 0),
                gr.stream_to_vector(gr.sizeof_gr_complex, fft_len),
                rx_fft, serializer, sink
        )
        self.tb.connect(offset_sig, (mixer, 1))
        self.tb.run ()
        # FIXME check this
        #self.assertEqual(sink.data(), tx_data)

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
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(n_syms)
        tag2 = gr.gr_tag_t()
        tag2.offset = 0
        tag2.key = pmt.pmt_string_to_symbol("packet_len")
        tag2.value = pmt.pmt_from_long(len(expected_result))
        src = gr.vector_source_c(tx_symbols, False, fft_len, (tag, tag2))
        serializer = digital.ofdm_serializer_vcc(fft_len, occupied_carriers, tag_name, "packet_len", 0, False)
        sink = gr.vector_sink_c()
        self.tb.connect(src, serializer, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_result)
        self.assertEqual(len(sink.tags()), 1)
        result_tag = sink.tags()[0]
        self.assertEqual(pmt.pmt_symbol_to_string(result_tag.key), "packet_len")
        self.assertEqual(pmt.pmt_to_long(result_tag.value), len(expected_result))

    def test_099 (self):
        """ Make sure it fails if it should """
        fft_len = 16
        occupied_carriers = ((1, 3, 4, 11, 12, 17),)
        tag_name = "len"
        self.assertRaises(RuntimeError, digital.ofdm_serializer_vcc, fft_len, occupied_carriers, tag_name)


if __name__ == '__main__':
    #gr_unittest.run(qa_ofdm_serializer_vcc, "qa_ofdm_serializer_vcc.xml")
    gr_unittest.run(qa_ofdm_serializer_vcc)

