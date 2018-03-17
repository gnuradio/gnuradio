#!/usr/bin/env python
# Copyright 2012, 2018 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks
import pmt

class qa_digital_carrier_allocator_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.tsb_key = "ts_last"

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """
        pretty simple (the carrier allocation here is not a practical OFDM configuration!)
        """
        fft_len = 6
        tx_symbols = (1, 2, 3)
        #             ^ this gets mapped to the DC carrier because occupied_carriers[0][0] == 0
        pilot_symbols = ((1j,),)
        occupied_carriers = ((0, 1, 2),)
        pilot_carriers = ((3,),)
        sync_word = (range(fft_len),)
        expected_result = tuple(sync_word[0] + [1j, 0, 0, 1, 2, 3])
        #                                                 ^ DC carrier
        src = blocks.vector_source_c(tx_symbols, False, 1)
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols, sync_word,
                       self.tsb_key)
        sink = blocks.tsb_vector_sink_c(vlen=fft_len, tsb_key=self.tsb_key)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_symbols), self.tsb_key),
                alloc,
                sink
        )
        self.tb.run()
        self.assertEqual(sink.data()[0], expected_result)

    def test_001_t2 (self):
        """
        pretty simple (same as before, but odd fft len)
        """
        fft_len = 5
        tx_symbols = (1, 2, 3)
        #             ^ this gets mapped to the DC carrier because occupied_carriers[0][0] == 0
        occupied_carriers = ((0, 1, 2),)
        pilot_carriers = ((-2,),)
        pilot_symbols = ((1j,),)
        expected_result = (1j, 0, 1, 2, 3)
        #                         ^ DC carrier
        src = blocks.vector_source_c(tx_symbols, False, 1)
        alloc = digital.ofdm_carrier_allocator_cvc(
                fft_len,
                occupied_carriers,
                pilot_carriers,
                pilot_symbols, (),
                self.tsb_key
        )
        sink = blocks.tsb_vector_sink_c(vlen=fft_len, tsb_key=self.tsb_key)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_symbols), self.tsb_key), alloc, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)

    def test_002_t (self):
        """
        same, but using negative carrier indices
        """
        fft_len = 6
        tx_symbols = (1, 2, 3)
        pilot_symbols = ((1j,),)
        occupied_carriers = ((-1, 1, 2),)
        pilot_carriers = ((3,),)
        expected_result = (1j, 0, 1, 0, 2, 3)
        src = blocks.vector_source_c(tx_symbols, False, 1)
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols, (),
                       self.tsb_key)
        sink = blocks.tsb_vector_sink_c(fft_len)
        self.tb.connect(
                src,
                blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_symbols), self.tsb_key),
                alloc,
                sink
        )
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)

    def test_002_t (self):
        """
        once again, but this time add a sync word
        """
        fft_len = 6
        sync_word = (0,) * fft_len
        tx_symbols = (1, 2, 3, 4, 5, 6)
        pilot_symbols = ((1j,),)
        occupied_carriers = ((-1, 1, 2),)
        pilot_carriers = ((3,),)
        expected_result = sync_word + (1j, 0, 1, 0, 2, 3) + (1j, 0, 4, 0, 5, 6)
        special_tag1 = gr.tag_t()
        special_tag1.offset = 0
        special_tag1.key = pmt.string_to_symbol("spam")
        special_tag1.value = pmt.to_pmt(23)
        special_tag2 = gr.tag_t()
        special_tag2.offset = 4
        special_tag2.key = pmt.string_to_symbol("eggs")
        special_tag2.value = pmt.to_pmt(42)
        src = blocks.vector_source_c(
            tx_symbols, False, 1,
            (special_tag1, special_tag2)
        )
        alloc = digital.ofdm_carrier_allocator_cvc(
            fft_len,
            occupied_carriers,
            pilot_carriers,
            pilot_symbols,
            sync_words=(sync_word,),
            len_tag_key=self.tsb_key
        )
        sink = blocks.tsb_vector_sink_c(fft_len)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_symbols), self.tsb_key), alloc, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)
        tags = [gr.tag_to_python(x) for x in sink.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        tags_expected = [
                (0, 'spam', 23),
                (2, 'eggs', 42),
        ]
        self.assertEqual(tags, tags_expected)

    def test_003_t (self):
        """
        more advanced:
        - 6 symbols per carrier
        - 2 pilots per carrier
        - have enough data for nearly 3 OFDM symbols
        - send that twice
        - add some random tags
        - don't shift
        """
        tx_symbols = range(1, 16); # 15 symbols
        pilot_symbols = ((1j, 2j), (3j, 4j))
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        pilot_carriers = ((2, 13), (3, 12))
        expected_result = (0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,  0,
                           0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12, 0,
                           0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0,  0)
        fft_len = 16
        testtag1 = gr.tag_t()
        testtag1.offset = 0
        testtag1.key = pmt.string_to_symbol('tag1')
        testtag1.value = pmt.from_long(0)
        testtag2 = gr.tag_t()
        testtag2.offset = 7 # On the 2nd OFDM symbol
        testtag2.key = pmt.string_to_symbol('tag2')
        testtag2.value = pmt.from_long(0)
        testtag3 = gr.tag_t()
        testtag3.offset = len(tx_symbols)+1 # First OFDM symbol of packet 2
        testtag3.key = pmt.string_to_symbol('tag3')
        testtag3.value = pmt.from_long(0)
        testtag4 = gr.tag_t()
        testtag4.offset = 2*len(tx_symbols)-1 # Last OFDM symbol of packet 2
        testtag4.key = pmt.string_to_symbol('tag4')
        testtag4.value = pmt.from_long(0)
        src = blocks.vector_source_c(tx_symbols * 2, False, 1, (testtag1, testtag2, testtag3, testtag4))
        alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols, (),
                       self.tsb_key,
                       False)
        sink = blocks.tsb_vector_sink_c(fft_len)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, 1, len(tx_symbols), self.tsb_key), alloc, sink)
        self.tb.run ()
        self.assertEqual(sink.data()[0], expected_result)
        tags_found = {'tag1': False, 'tag2': False, 'tag3': False, 'tag4': False}
        correct_offsets = {'tag1': 0, 'tag2': 1, 'tag3': 3, 'tag4': 5}
        for tag in sink.tags():
            key = pmt.symbol_to_string(tag.key)
            if key in tags_found.keys():
                tags_found[key] = True
                self.assertEqual(correct_offsets[key], tag.offset)
        self.assertTrue(all(tags_found.values()))

    def test_004_t (self):
        """
        Provoking RuntimeError exceptions providing wrong user input (earlier invisible SIGFPE).
        """
        fft_len = 6

        # Occupied carriers
        with self.assertRaises(RuntimeError) as oc:
          alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                        (),
                        ((),),
                        ((),),
                        (),
                        self.tsb_key)

        # Pilot carriers
        with self.assertRaises(RuntimeError) as pc:
          alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                        ((),),
                        (),
                        ((),),
                        (),
                        self.tsb_key)

        # Pilot carrier symbols
        with self.assertRaises(RuntimeError) as ps:
          alloc = digital.ofdm_carrier_allocator_cvc(fft_len,
                        ((),),
                        ((),),
                        (),
                        (),
                        self.tsb_key)


        self.assertEqual(str(oc.exception), "Occupied carriers must be of type vector of vector i.e. ((),).")
        self.assertEqual(str(pc.exception), "Pilot carriers must be of type vector of vector i.e. ((),).")
        self.assertEqual(str(ps.exception), "Pilot symbols must be of type vector of vector i.e. ((),).")


if __name__ == '__main__':
    gr_unittest.run(qa_digital_carrier_allocator_cvc, "qa_digital_carrier_allocator_cvc.xml")

