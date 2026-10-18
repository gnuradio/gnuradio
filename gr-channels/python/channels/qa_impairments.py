#!/usr/bin/env python
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, channels
import numpy


class test_impairments(gr_unittest.TestCase):

    def run_block(self, op, x, complex_io=True):
        tb = gr.top_block()
        if complex_io:
            src = blocks.vector_source_c(list(x))
            snk = blocks.vector_sink_c()
        else:
            src = blocks.vector_source_f(list(x))
            snk = blocks.vector_sink_f()
        tb.connect(src, op, snk)
        tb.run()
        return numpy.array(snk.data())

    def tone(self, n=20000):
        return numpy.exp(2j * numpy.pi * 0.01 * numpy.arange(n))

    def test_quantizer(self):
        x = [-3.0, -1.0, -0.5, 0.3, 0.999, 1.0, 2.0]
        y = self.run_block(channels.quantizer(8), x, complex_io=False)
        expected = [-1.0, -1.0, -0.5, 38 / 128, 127 / 128, 127 / 128, 127 / 128]
        self.assertFloatTuplesAlmostEqual(y, expected, 6)
        with self.assertRaises(ValueError):
            channels.quantizer(17)

    def test_amp_bal(self):
        with self.assertRaises(ValueError):
            channels.amp_bal(0)
        x = self.tone()
        x = x.real + 0.5j * x.imag
        y = self.run_block(channels.amp_bal(), x)
        self.assertFalse(numpy.any(numpy.isnan(y)))
        tail = y[-5000:]
        self.assertAlmostEqual(numpy.std(tail.imag) / numpy.std(tail.real), 1.0,
                               delta=0.05)

    def test_amp_bal_zero_padded(self):
        # Q is zero for the first 1000 samples
        x = self.tone()
        x = numpy.concatenate((numpy.zeros(1000), x.real + 0.5j * x.imag))
        for alpha in (1e-4, 0.5, 1.0):
            y = self.run_block(channels.amp_bal(alpha), x)
            self.assertFalse(numpy.any(numpy.isnan(y)))
            self.assertEqual(numpy.max(numpy.abs(y[:1000])), 0)

    def test_impairments_default_is_transparent(self):
        x = self.tone()
        y = self.run_block(channels.impairments(), x)
        self.assertLess(numpy.max(numpy.abs(y - x)), 1e-4)

    def test_phase_noise_gen(self):
        x = numpy.ones(100000, dtype=complex)
        mag, alpha = 0.5, 0.1
        a = self.run_block(channels.phase_noise_gen(mag, alpha, 3), x)
        b = self.run_block(channels.phase_noise_gen(mag, alpha, 3), x)
        c = self.run_block(channels.phase_noise_gen(mag, alpha, 4), x)
        self.assertComplexTuplesAlmostEqual(a, b, 6)
        self.assertGreater(numpy.max(numpy.abs(a - c)), 0.01)
        self.assertAlmostEqual(numpy.std(numpy.angle(a)),
                               mag * numpy.sqrt(alpha / (2 - alpha)), delta=0.01)

    def test_distortion_2_gen(self):
        beta = 0.1
        x = self.tone() * 0.8
        y = self.run_block(channels.distortion_2_gen(beta), x)
        self.assertComplexTuplesAlmostEqual(y, x + beta * numpy.abs(x)**2, 5)

    def test_distortion_3_gen(self):
        beta = 0.1
        x = self.tone() * 0.8
        y = self.run_block(channels.distortion_3_gen(beta), x)
        self.assertComplexTuplesAlmostEqual(y, x + beta * numpy.abs(x)**2 * x, 5)


if __name__ == '__main__':
    gr_unittest.run(test_impairments)
