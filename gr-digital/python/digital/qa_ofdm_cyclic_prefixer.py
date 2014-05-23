#!/usr/bin/env python
#
# Copyright 2007,2010,2011,2013,2014 Free Software Foundation, Inc.
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
        cp_len = 2
        expected_result = (6, 7, 0, 1, 2, 3, 4, 5, 6, 7,
                           6, 7, 0, 1, 2, 3, 4, 5, 6, 7)
        src = blocks.vector_source_c(range(fft_len) * 2, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, fft_len + cp_len)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_wo_tags_2s_rolloff(self):
        " No tags, but have a 2-sample rolloff "
        fft_len = 8
        cp_len = 2
        rolloff = 2
        expected_result = (7.0/2,       8, 1, 2, 3, 4, 5, 6, 7, 8, # 1.0/2
                           7.0/2+1.0/2, 8, 1, 2, 3, 4, 5, 6, 7, 8)
        src = blocks.vector_source_c(range(1, fft_len+1) * 2, False, fft_len)
        cp = digital.ofdm_cyclic_prefixer(fft_len, fft_len + cp_len, rolloff)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, cp, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_result)

    def test_with_tags_2s_rolloff(self):
        " With tags and a 2-sample rolloff "
        fft_len = 8
        cp_len = 2
        tag_name = "ts_last"
        expected_result = (7.0/2,       8, 1, 2, 3, 4, 5, 6, 7, 8, # 1.0/2
                           7.0/2+1.0/2, 8, 1, 2, 3, 4, 5, 6, 7, 8, 1.0/2)
        tag2 = gr.tag_t()
        tag2.offset = 1
        tag2.key = pmt.string_to_symbol("random_tag")
        tag2.value = pmt.from_long(42)
        src = blocks.vector_source_c(range(1, fft_len+1) * 2, False, fft_len, (tag2,))
        cp = digital.ofdm_cyclic_prefixer(fft_len, fft_len + cp_len, 2, tag_name)
        sink = blocks.tsb_vector_sink_c(tsb_key=tag_name)
        self.tb.connect(src, blocks.stream_to_tagged_stream(gr.sizeof_gr_complex, fft_len, 2, tag_name), cp, sink)
        self.tb.run()
        self.assertEqual(sink.data()[0], expected_result)
        tags = [gr.tag_to_python(x) for x in sink.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        expected_tags = [
            (fft_len+cp_len, "random_tag", 42)
        ]
        self.assertEqual(tags, expected_tags)


if __name__ == '__main__':
    gr_unittest.run(test_ofdm_cyclic_prefixer, "test_ofdm_cyclic_prefixer.xml")

