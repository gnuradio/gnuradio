#!/usr/bin/env python
#
# Copyright 2007,2010,2012 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, analog, blocks
import numpy

class test_fastnoise_source(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_test_moments(self):
        tb = self.tb

        NUM = 2**22
        NUM_ITEMS = 10**6
        DEFAULTARGS = {"samples": NUM, "seed": 43, "ampl": 1}
        self.uni_real_src = analog.fastnoise_source_f(type = analog.GR_UNIFORM, **DEFAULTARGS)
        self.uni_cplx_src = analog.fastnoise_source_c(type = analog.GR_UNIFORM, **DEFAULTARGS)

        self.norm_real_src = analog.fastnoise_source_f(type = analog.GR_GAUSSIAN, **DEFAULTARGS)
        self.norm_cplx_src = analog.fastnoise_source_c(type = analog.GR_GAUSSIAN, **DEFAULTARGS)

        self.lapl_real_src = analog.fastnoise_source_f(type = analog.GR_LAPLACIAN, **DEFAULTARGS)

        types = {
            self.uni_real_src: "uniform", self.uni_cplx_src: "uniform",
            self.norm_cplx_src: "norm", self.norm_cplx_src: "norm",
            self.lapl_real_src: "laplace"
        }

        real_srcs = [self.uni_real_src, self.norm_real_src, self.lapl_real_src]
        cplx_srcs = [self.uni_cplx_src, self.norm_cplx_src]

        paths = {}
        for src in real_srcs:
            head = blocks.head(nitems=NUM_ITEMS, sizeof_stream_item=gr.sizeof_float)
            sink = blocks.vector_sink_f()
            paths[src] = (src, head, sink)

        for src in cplx_srcs:
            head = blocks.head(nitems=NUM_ITEMS, sizeof_stream_item=gr.sizeof_gr_complex)
            sink = blocks.vector_sink_c()
            paths[src] = (src, head, sink)
        for path in paths.itervalues():
            tb.connect(*path)

        tb.run()
        self.data = {src: path[2].data() for src, path in paths.iteritems()}
        data = self.data
        # min, max
        self.assertAlmostEqual(min(data[self.uni_real_src]), -1, places=4)
        self.assertAlmostEqual(max(data[self.uni_real_src]), 1, places=4)
        # mean, variance
        data = numpy.array(data[self.uni_real_src])
        self.assertAlmostEqual(data.mean(), 0, places = 2)
        self.assertAlmostEqual(data.var(), (1-(-1))**2./12, places = 3)
        data = numpy.array(self.data[self.norm_real_src])
        self.assertAlmostEqual(data.mean(), 0, places=2)
        self.assertAlmostEqual(data.var(), 1, places=2)
        data = numpy.array(self.data[self.lapl_real_src])
        self.assertAlmostEqual(data.mean(), 0, places=2)
        self.assertAlmostEqual(data.var(), 2, places=2)

if __name__ == '__main__':
    gr_unittest.run(test_fastnoise_source, "test_fastnoise_source.xml")

