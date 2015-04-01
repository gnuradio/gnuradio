#!/usr/bin/env python
#
# Copyright 2007,2010,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks

class test_peak_detector2(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_regen1(self):
        # test that the new peak_detector2 works and the previous peak_detector2 fails
        tb = self.tb
        l=8100
        n=20
        m = 100
        data = l*(0,)+ (10,)+ m*(1,)+(100,)+ n*(1,)
        expected_result = (l+1+m)*(0L,)+(1L,) +n*(0L,)

        src = blocks.vector_source_f(data, False)
        regen = blocks.peak_detector2_fb(5, (m+10), 0.1)
        nulls = blocks.null_sink(gr.sizeof_float)
        dst = blocks.vector_sink_b()
        tb.connect(src, regen)
        tb.connect((regen,1), nulls)
        tb.connect((regen,0), dst)
        
        tb.run()
        dst_data = dst.data()
        
        self.assertEqual(len(expected_result), len(dst_data))
        self.assertEqual(expected_result, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_peak_detector2, "test_peak_detector2.xml")
