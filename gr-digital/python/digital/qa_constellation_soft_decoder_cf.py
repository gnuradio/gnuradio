#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
from math import sqrt

class test_constellation_soft_decoder(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_constellation_soft_decoder_cf_bpsk(self):
        prec = 8
  	src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j, 1+1j)
        lut = digital.soft_dec_table_generator(digital.sd_psk_2_0x0, prec)
	expected_result = list()
        for s in src_data:
            expected_result += digital.calc_soft_dec_from_table(s, lut, prec, Es=2/sqrt(2))

        cnst_pts, code = digital.psk_2_0x0()
        cnst = digital.constellation_calcdist(cnst_pts, code, 2, 1)
        cnst.set_soft_dec_lut(lut, int(prec))
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_soft_decoder_cf(cnst.base())
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 4)

    def test_constellation_soft_decoder_cf_qpsk(self):
        prec = 8
  	src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j, 1+1j)
        lut = digital.soft_dec_table_generator(digital.sd_psk_4_0x0_0_1, prec)
	expected_result = list()
        for s in src_data:
            expected_result += digital.calc_soft_dec_from_table(s, lut, prec)

        cnst_pts,code = digital.psk_4_0x0_0_1()
        cnst = digital.constellation_calcdist(cnst_pts, code, 2, 1)
        cnst.set_soft_dec_lut(lut, int(prec))
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_soft_decoder_cf(cnst.base())
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 5)

    def test_constellation_soft_decoder_cf_qam16(self):
        prec = 8
  	src_data = (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j, 1+1j)
        lut = digital.soft_dec_table_generator(digital.sd_qam_16_0x0_0_1_2_3, prec)
	expected_result = list()
        for s in src_data:
            expected_result += digital.calc_soft_dec_from_table(s, lut, prec)

        cnst_pts = digital.qam_16_0x0_0_1_2_3()
        cnst = digital.constellation_calcdist(cnst_pts[0], cnst_pts[1], 2, 1)
        cnst.set_soft_dec_lut(lut, int(prec))
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_soft_decoder_cf(cnst.base())
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 5)


if __name__ == '__main__':
    gr_unittest.run(test_constellation_soft_decoder, "test_constellation_soft_decoder.xml")

