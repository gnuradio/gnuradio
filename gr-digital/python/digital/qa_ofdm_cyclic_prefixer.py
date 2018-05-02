#!/usr/bin/env python
#
# Copyright 2007-2018 Free Software Foundation, Inc.
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

class test_ofdm_cyclic_prefixer (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_wo_tags_no_rolloff(self):
        " The easiest test: make sure the CP is added correctly. "
        fft_len = 8
        cp_lengths = (2,)
        expected_result = (6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                           6, 7, 0, 1, 2, 3, 4, 5, 6, 7)
        src = blocks.vector_source_c(range(fft_len) * 2, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_wo_tags_2s_rolloff(self):
        " No tags, but have a 2-sample rolloff "
        fft_len = 8
        cp_lengths = (2,)
        rolloff = 2
        expected_result = (7.0/2,       8, 1, 2, 3, 4, 5, 6, 7, 8, # 1.0/2
                           7.0/2+1.0/2, 8, 1, 2, 3, 4, 5, 6, 7, 8)
        src = blocks.vector_source_c(range(1, fft_len+1) * 2, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths, rolloff)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_with_tags_2s_rolloff(self):
        " With tags and a 2-sample rolloff "
        fft_len = 8
        cp_lengths = (2,)
        rolloff = 2
        tag_name = "ts_last"
        expected_result = (7.0/2,       8, 1, 2, 3, 4, 5, 6, 7, 8, # 1.0/2
                           7.0/2+1.0/2, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1.0/2)
        tag = gr.tag_t()
        tag.offset = 1
        tag.key = pmt.string_to_symbol("random_tag")
        tag.value = pmt.from_long(42)
        src = blocks.vector_source_c(range(1, fft_len+1) * 2, False, fft_len, (tag,))
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths, rolloff, tag_name)
        sink = blocks.tsb_vector_sink_c(tsb_key=tag_name)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, 2, tag_name), cp, sink)
        self.tb.run()
        self.assertEqual(sink.data()[0], expected_result)
        tags = [gr.tag_to_python(x) for x in sink.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        expected_tags = [
            (fft_len+cp_lengths[0], "random_tag", 42)
        ]
        self.assertEqual(tags, expected_tags)

    def test_wo_tags_no_rolloff_multiple_cps(self):
        "Two CP lengths, no rolloff and no tags."
        fft_len = 8
        cp_lengths = (3, 2, 2)
        expected_result = ( 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, # 1
                            6, 7, 0, 1, 2, 3, 4, 5, 6, 7,    # 2
                            6, 7, 0, 1, 2, 3, 4, 5, 6, 7,    # 3
                            5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, # 4
                            6, 7, 0, 1, 2, 3, 4, 5, 6, 7     # 5
        )
        src = blocks.vector_source_c(range(fft_len)*5, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_wo_tags_2s_rolloff_multiple_cps(self):
        "Two CP lengths, 2-sample rolloff and no tags."
        fft_len = 8
        cp_lengths = (3, 2, 2)
        rolloff = 2
        expected_result = ( 6.0/2,7,8,1,2,3,4,5,6,7,8,        #1
                            7.0/2 + 1.0/2,8,1,2,3,4,5,6,7,8,  #2
                            7.0/2 + 1.0/2,8,1,2,3,4,5,6,7,8,  #3
                            6.0/2 + 1.0/2,7,8,1,2,3,4,5,6,7,8,#4
                            7.0/2 + 1.0/2,8,1,2,3,4,5,6,7,8   #5
        )
        src = blocks.vector_source_c(range(1, fft_len+1)*5, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths, rolloff)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_with_tags_2s_rolloff_multiples_cps(self):
        "Two CP lengths, 2-sample rolloff and tags."
        fft_len = 8
        cp_lengths = (3, 2, 2)
        rolloff = 2
        tag_name = "ts_last"
        expected_result = (6.0/2,7,8,1,2,3,4,5,6,7,8,           #1
                           7.0/2+1.0/2,8,1,2,3,4,5,6,7,8,1.0/2  #Last tail
        )
        # First test tag
        tag0 = gr.tag_t()
        tag0.offset = 0
        tag0.key = pmt.string_to_symbol("first_tag")
        tag0.value = pmt.from_long(24)
        # Second test tag
        tag1 = gr.tag_t()
        tag1.offset = 1
        tag1.key = pmt.string_to_symbol("second_tag")
        tag1.value = pmt.from_long(42)
        src = blocks.vector_source_c(range(1, fft_len+1) * 2, False, fft_len, (tag0, tag1))
        cp = digital.ofdm_cyclic_prefixer(fft_len, cp_lengths, rolloff, tag_name)
        sink = blocks.tsb_vector_sink_c(tsb_key=tag_name)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, 2, tag_name), cp, sink)
        self.tb.run()
        self.assertEqual(sink.data()[0], expected_result)
        tags = [gr.tag_to_python(x) for x in sink.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        expected_tags = [
            (0, "first_tag", 24),
            (fft_len + cp_lengths[0], "second_tag", 42)
        ]
        self.assertEqual(tags, expected_tags)

    def test_wo_tags_no_rolloff_old_api(self):
        "Test the old API."
        fft_len = 8
        cp_len = 2
        expected_result = (6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                           6, 7, 0, 1, 2, 3, 4, 5, 6, 7)
        src = blocks.vector_source_c(range(fft_len) * 2, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, fft_len + cp_len)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

if __name__ == '__main__':
    gr_unittest.run(test_ofdm_cyclic_prefixer, "test_ofdm_cyclic_prefixer.xml")

