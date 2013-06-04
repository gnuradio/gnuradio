#!/usr/bin/env python
#
# Copyright 2010,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, analog, blocks

class test_cpm(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def do_check_phase_shift(self, type, name):
        sps = 2
        L = 1
        in_bits = (1,) * 20
        src = blocks.vector_source_b(in_bits, False)
        cpm = digital.cpmmod_bc(type, 0.5, sps, L)
        arg = blocks.complex_to_arg()
        sink = blocks.vector_sink_f()

        self.tb.connect(src, cpm, arg, sink)
        self.tb.run()

        symbol_phases = numpy.array(sink.data()[sps*L-1::sps])
        phase_diff = numpy.mod(numpy.subtract(symbol_phases[1:], symbol_phases[:-1]),
                               (2*numpy.pi,) * (len(symbol_phases)-1))
        self.assertFloatTuplesAlmostEqual(tuple(phase_diff), (0.5 * numpy.pi,) * len(phase_diff), 5,
                                          msg="Phase shift was not correct for CPM method " + name)

    def test_001_lrec(self):
        self.do_check_phase_shift(analog.cpm.LRC, 'LREC')

    def test_001_lrc(self):
        self.do_check_phase_shift(analog.cpm.LRC, 'LRC')

    def test_001_lsrc(self):
        self.do_check_phase_shift(analog.cpm.LSRC, 'LSRC')

    def test_001_ltfm(self):
        self.do_check_phase_shift(analog.cpm.TFM, 'TFM')

    def test_001_lgmsk(self):
        sps = 2
        L = 5
        bt = 0.3
        in_bits = (1,) * 20
        src = blocks.vector_source_b(in_bits, False)
        gmsk = digital.gmskmod_bc(sps, L, bt)
        arg = blocks.complex_to_arg()
        sink = blocks.vector_sink_f()

        self.tb.connect(src, gmsk, arg, sink)
        self.tb.run()

        symbol_phases = numpy.array(sink.data()[sps*L-1::sps])
        phase_diff = numpy.mod(numpy.subtract(symbol_phases[1:], symbol_phases[:-1]),
                               (2*numpy.pi,) * (len(symbol_phases)-1))
        self.assertFloatTuplesAlmostEqual(tuple(phase_diff), (0.5 * numpy.pi,) * len(phase_diff), 5,
                                          msg="Phase shift was not correct for GMSK")

    def test_phase_response(self):
        phase_response = analog.cpm.phase_response(analog.cpm.LREC, 2, 4)
        self.assertAlmostEqual(numpy.sum(phase_response), 1)


if __name__ == '__main__':
    gr_unittest.run(test_cpm, "test_cpm.xml")

