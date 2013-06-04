#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, filter, blocks

class test_dc_blocker(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        ''' Test impulse response - long form, cc '''
        src_data = [1,] + 100*[0,]
        expected_result = ((-0.02072429656982422+0j), (-0.02081298828125+0j),
                           (0.979156494140625+0j), (-0.02081298828125+0j),
                           (-0.02072429656982422+0j))

        src = blocks.vector_source_c(src_data)
        op = filter.dc_blocker_cc(32, True)
        dst = blocks.vector_sink_c()

        self.tb.connect (src, op, dst)
        self.tb.run()

        # only test samples around 2D-2
        result_data = dst.data()[60:65]
        self.assertComplexTuplesAlmostEqual (expected_result, result_data)

    def test_002(self):
        ''' Test impulse response - short form, cc '''
        src_data = [1,] + 100*[0,]
        expected_result = ((-0.029296875+0j), (-0.0302734375+0j),
                           (0.96875+0j), (-0.0302734375+0j),
                           (-0.029296875+0j))

        src = blocks.vector_source_c(src_data)
        op = filter.dc_blocker_cc(32, False)
        dst = blocks.vector_sink_c()

        self.tb.connect (src, op, dst)
        self.tb.run()

        # only test samples around D-1
        result_data = dst.data()[29:34]
        self.assertComplexTuplesAlmostEqual (expected_result, result_data)


    def test_003(self):
        ''' Test impulse response - long form, ff '''
        src_data = [1,] + 100*[0,]
        expected_result = ((-0.02072429656982422), (-0.02081298828125),
                           (0.979156494140625), (-0.02081298828125),
                           (-0.02072429656982422))

        src = blocks.vector_source_f(src_data)
        op = filter.dc_blocker_ff(32, True)
        dst = blocks.vector_sink_f()

        self.tb.connect (src, op, dst)
        self.tb.run()

        # only test samples around 2D-2
        result_data = dst.data()[60:65]
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_004(self):
        ''' Test impulse response - short form, ff '''
        src_data = [1,] + 100*[0,]
        expected_result = ((-0.029296875), (-0.0302734375),
                           (0.96875), (-0.0302734375),
                           (-0.029296875))

        src = blocks.vector_source_f(src_data)
        op = filter.dc_blocker_ff(32, False)
        dst = blocks.vector_sink_f()

        self.tb.connect (src, op, dst)
        self.tb.run()

        # only test samples around D-1
        result_data = dst.data()[29:34]
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_dc_blocker, "test_dc_blocker.xml")

