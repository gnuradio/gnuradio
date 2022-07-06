#!/usr/bin/env python
#
# Copyright 2007,2010,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, analog, blocks
import numpy


class test_fastnoise_source(gr_unittest.TestCase):

    def setUp(self):

        self.num = 2**22
        self.num_items = 10**6
        self.default_args = {"samples": self.num, "seed": 43, "ampl": 1}

    def tearDown(self):
        pass

    def run_test_real(self, form):
        """ Run test case with float input/output
        """
        tb = gr.top_block()
        src = analog.fastnoise_source_f(type=form, **self.default_args)
        head = blocks.head(
            nitems=self.num_items,
            sizeof_stream_item=gr.sizeof_float)
        sink = blocks.vector_sink_f()
        tb.connect(src, head, sink)
        tb.run()
        return numpy.array(sink.data())

    def run_test_complex(self, form):
        """ Run test case with complex input/output
        """
        tb = gr.top_block()
        src = analog.fastnoise_source_c(type=form, **self.default_args)
        head = blocks.head(
            nitems=self.num_items,
            sizeof_stream_item=gr.sizeof_gr_complex)
        sink = blocks.vector_sink_c()
        tb.connect(src, head, sink)
        tb.run()
        return numpy.array(sink.data())

    def test_000_real_negative_seed_instantiation(self):
        _ = analog.fastnoise_source_f(analog.noise_type_t.GR_GAUSSIAN,
                                      2.0,
                                      -666,
                                      128)

    def test_000_complex_negative_seed_instantiation(self):
        _ = analog.fastnoise_source_c(analog.noise_type_t.GR_GAUSSIAN,
                                      2.0,
                                      -666,
                                      128)

    def test_000_real_64bit_seed_instantiation(self):
        _ = analog.fastnoise_source_f(analog.noise_type_t.GR_GAUSSIAN,
                                      2.0,
                                      0xFFFFFFFFFFFFFFFF,
                                      128)

    def test_000_complex_64bit_seed_instantiation(self):
        _ = analog.fastnoise_source_f(analog.noise_type_t.GR_GAUSSIAN,
                                      2.0,
                                      0xFFFFFFFFFFFFFFFF,
                                      128)

    def test_001_real_uniform_moments(self):

        data = self.run_test_real(analog.GR_UNIFORM)

        self.assertAlmostEqual(min(data), -1, places=4)
        self.assertAlmostEqual(max(data), 1, places=4)

        # mean, variance
        self.assertAlmostEqual(data.mean(), 0, places=2)
        self.assertAlmostEqual(data.var(), (1 - (-1))**2. / 12, places=3)

    def test_001_real_gaussian_moments(self):
        data = self.run_test_real(analog.GR_GAUSSIAN)

        # mean, variance
        self.assertAlmostEqual(data.mean(), 0, places=2)
        self.assertAlmostEqual(data.var(), 1, places=2)

    def test_001_real_laplacian_moments(self):
        data = self.run_test_real(analog.GR_LAPLACIAN)

        # mean, variance
        self.assertAlmostEqual(data.mean(), 0, places=2)
        self.assertAlmostEqual(data.var(), 2, places=2)

    def test_001_complex_uniform_moments(self):
        data = self.run_test_complex(analog.GR_UNIFORM)

        # mean, variance
        self.assertAlmostEqual(data.real.mean(), 0, places=2)
        self.assertAlmostEqual(data.real.var(), 0.5 *
                               (1 - (-1))**2. / 12, places=3)

        self.assertAlmostEqual(data.imag.mean(), 0, places=2)
        self.assertAlmostEqual(data.imag.var(), 0.5 *
                               (1 - (-1))**2. / 12, places=3)

    def test_001_complex_gaussian_moments(self):
        data = self.run_test_complex(analog.GR_GAUSSIAN)

        # mean, variance
        self.assertAlmostEqual(data.real.mean(), 0, places=2)
        self.assertAlmostEqual(data.real.var(), 0.5, places=2)

        self.assertAlmostEqual(data.imag.mean(), 0, places=2)
        self.assertAlmostEqual(data.imag.var(), 0.5, places=2)

    def test_002_real_uniform_reproducibility(self):
        data1 = self.run_test_real(analog.GR_UNIFORM)
        data2 = self.run_test_real(analog.GR_UNIFORM)

        # It's pseudoramdo thus must be equal
        self.assertTrue(numpy.array_equal(data1, data2))

    def test_002_real_gaussian_reproducibility(self):
        data1 = self.run_test_real(analog.GR_GAUSSIAN)
        data2 = self.run_test_real(analog.GR_GAUSSIAN)

        self.assertTrue(numpy.array_equal(data1, data2))

    def test_003_real_uniform_pool(self):
        src = analog.fastnoise_source_f(
            type=analog.GR_UNIFORM, **self.default_args)
        src2 = analog.fastnoise_source_f(
            type=analog.GR_UNIFORM, **self.default_args)
        self.assertTrue(
            numpy.array_equal(
                numpy.array(
                    src.samples()), numpy.array(
                    src2.samples())))

    def test_003_real_gaussian_pool(self):
        src = analog.fastnoise_source_f(
            type=analog.GR_GAUSSIAN, **self.default_args)
        src2 = analog.fastnoise_source_f(
            type=analog.GR_GAUSSIAN, **self.default_args)
        self.assertTrue(
            numpy.array_equal(
                numpy.array(
                    src.samples()), numpy.array(
                    src2.samples())))

    def test_003_cmplx_gaussian_pool(self):
        src = analog.fastnoise_source_c(
            type=analog.GR_GAUSSIAN, **self.default_args)
        src2 = analog.fastnoise_source_c(
            type=analog.GR_GAUSSIAN, **self.default_args)
        self.assertTrue(
            numpy.array_equal(
                numpy.array(
                    src.samples()), numpy.array(
                    src2.samples())))

    def test_003_cmplx_uniform_pool(self):
        src = analog.fastnoise_source_c(
            type=analog.GR_UNIFORM, **self.default_args)
        src2 = analog.fastnoise_source_c(
            type=analog.GR_UNIFORM, **self.default_args)
        self.assertTrue(
            numpy.array_equal(
                numpy.array(
                    src.samples()), numpy.array(
                    src2.samples())))

    def test_003_real_laplacian_pool(self):
        src = analog.fastnoise_source_f(
            type=analog.GR_LAPLACIAN, **self.default_args)
        src2 = analog.fastnoise_source_f(
            type=analog.GR_LAPLACIAN, **self.default_args)
        self.assertTrue(
            numpy.array_equal(
                numpy.array(
                    src.samples()), numpy.array(
                    src2.samples())))


if __name__ == '__main__':
    gr_unittest.run(test_fastnoise_source)
