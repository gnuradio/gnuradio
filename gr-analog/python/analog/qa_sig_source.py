#!/usr/bin/env python
#
# Copyright 2004, 2007, 2010, 2012, 2013, 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math
import time
import pmt
from gnuradio import gr, gr_unittest, analog, blocks


class test_sig_source(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_const_f(self):
        tb = self.tb
        expected_result = [1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5]
        src1 = analog.sig_source_f(1e6, analog.GR_CONST_WAVE, 0, 1.5)
        op = blocks.head(gr.sizeof_float, 10)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

    def test_const_i(self):
        tb = self.tb
        expected_result = [1, 1, 1, 1]
        src1 = analog.sig_source_i(1e6, analog.GR_CONST_WAVE, 0, 1)
        op = blocks.head(gr.sizeof_int, 4)
        dst1 = blocks.vector_sink_i()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

    def test_const_b(self):
        tb = self.tb
        expected_result = [1, 1, 1, 1]
        src1 = analog.sig_source_b(1e6, analog.GR_CONST_WAVE, 0, 1)
        op = blocks.head(gr.sizeof_char, 4)
        dst1 = blocks.vector_sink_b()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

    def test_sine_f(self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        expected_result = [0, sqrt2, 1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0]
        src1 = analog.sig_source_f(8, analog.GR_SIN_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_float, 9)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_sine_b(self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        temp_result = [0, sqrt2, 1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0]
        amp = 8
        expected_result = tuple([int(z * amp) for z in temp_result])
        src1 = analog.sig_source_b(8, analog.GR_SIN_WAVE, 1.0, amp)
        op = blocks.head(gr.sizeof_char, 9)
        dst1 = blocks.vector_sink_b()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        # Let the python know we are dealing with signed int behind scenes
        dst_data_signed = [b if b < 127 else (256 - b) * -1 for b in dst_data]
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data_signed)

    def test_cosine_f(self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        expected_result = [1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0, sqrt2, 1]
        src1 = analog.sig_source_f(8, analog.GR_COS_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_float, 9)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_cosine_c(self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        sqrt2j = 1j * math.sqrt(2) / 2
        expected_result = [
            1, sqrt2 + sqrt2j, 1j, -sqrt2 + sqrt2j, -1, -sqrt2 - sqrt2j, -1j,
            sqrt2 - sqrt2j, 1
        ]
        src1 = analog.sig_source_c(8, analog.GR_COS_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_sqr_c(self):
        tb = self.tb  # arg6 is a bit before -PI/2
        expected_result = [1j, 1j, 0, 0, 1, 1, 1 + 0j, 1 + 1j, 1j]
        src1 = analog.sig_source_c(8, analog.GR_SQR_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

    def test_tri_c(self):
        tb = self.tb
        expected_result = [
            1 + .5j, .75 + .75j, .5 + 1j, .25 + .75j, 0 + .5j, .25 + .25j,
            .5 + 0j, .75 + .25j, 1 + .5j
        ]
        src1 = analog.sig_source_c(8, analog.GR_TRI_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_saw_c(self):
        tb = self.tb
        expected_result = [
            .5 + .25j, .625 + .375j, .75 + .5j, .875 + .625j, 0 + .75j,
            .125 + .875j, .25 + 1j, .375 + .125j, .5 + .25j
        ]
        src1 = analog.sig_source_c(8, analog.GR_SAW_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_sqr_f(self):
        tb = self.tb
        expected_result = [0, 0, 0, 0, 1, 1, 1, 1, 0]
        src1 = analog.sig_source_f(8, analog.GR_SQR_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_float, 9)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)

    def test_tri_f(self):
        tb = self.tb
        expected_result = [1, .75, .5, .25, 0, .25, .5, .75, 1]
        src1 = analog.sig_source_f(8, analog.GR_TRI_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_float, 9)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_saw_f(self):
        tb = self.tb
        expected_result = [.5, .625, .75, .875, 0, .125, .25, .375, .5]
        src1 = analog.sig_source_f(8, analog.GR_SAW_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_float, 9)
        dst1 = blocks.vector_sink_f()
        tb.connect(src1, op)
        tb.connect(op, dst1)
        tb.run()
        dst_data = dst1.data()
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_low_frequency_accuracy(self):
        # Only 4096 samples are needed to expose the 32-bit tuning error.
        sample_rate = 1e8
        count = 4096
        for frequency in (1.0, -1.0, 1e-3, -1e-3, 0.0):
            for waveform in (analog.GR_SIN_WAVE, analog.GR_COS_WAVE):
                for complex_output in (False, True):
                    with self.subTest(frequency=frequency,
                                      waveform=waveform,
                                      complex_output=complex_output):
                        tb = gr.top_block()
                        phase = 0.5 if waveform == analog.GR_COS_WAVE else 0.0
                        if complex_output:
                            src = analog.sig_source_c(sample_rate, waveform,
                                                      frequency, 1.0, 0, phase)
                            sink = blocks.vector_sink_c()
                            item_size = gr.sizeof_gr_complex
                        else:
                            src = analog.sig_source_f(sample_rate, waveform,
                                                      frequency, 1.0, 0, phase)
                            sink = blocks.vector_sink_f()
                            item_size = gr.sizeof_float
                        tb.connect(src, blocks.head(item_size, count), sink)
                        tb.run()
                        data = sink.data()
                        phase_change = 2 * math.pi * frequency * (
                            count - 1) / sample_rate
                        last_phase = phase + phase_change
                        if complex_output:
                            # Measure phase independently of the LUT's bounded
                            # amplitude interpolation error near cosine peaks.
                            rotation = data[-1] * data[0].conjugate()
                            measured = math.atan2(rotation.imag, rotation.real)
                            # Both output samples have single-precision components.
                            self.assertLess(abs(measured - phase_change), 1e-7)
                        else:
                            # Taking a difference cancels the initial LUT error.
                            if waveform == analog.GR_SIN_WAVE:
                                expected = math.sin(last_phase) - math.sin(
                                    phase)
                            else:
                                expected = math.cos(last_phase) - math.cos(
                                    phase)
                            self.assertLess(
                                abs((data[-1] - data[0]) - expected), 2e-7)

    def test_retune_preserves_phase(self):
        src = analog.sig_source_c(1e8, analog.GR_COS_WAVE, 1.0, 1.0)
        head = blocks.head(gr.sizeof_gr_complex, 4096)
        self.tb.connect(src, head, blocks.null_sink(gr.sizeof_gr_complex))
        self.tb.run()
        phase = src.phase()
        src.set_frequency(-1.0)
        self.assertEqual(src.phase(), phase)
        src.set_sampling_freq(2e8)
        self.assertEqual(src.phase(), phase)
        src.set_frequency(0.0)
        self.assertEqual(src.phase(), phase)

    def test_retune_flowgraph(self):
        sample_rate = 1e6
        count = 65536
        phase = 0.37
        new_frequency = -2345.678
        for complex_output in (False, True):
            with self.subTest(complex_output=complex_output):
                tb = gr.top_block()
                old_frequency = 1234.567 if complex_output else 0.0
                if complex_output:
                    src = analog.sig_source_c(sample_rate, analog.GR_COS_WAVE,
                                              old_frequency, 1.0, 0, phase)
                    sink = blocks.vector_sink_c()
                    item_size = gr.sizeof_gr_complex
                else:
                    src = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE,
                                              old_frequency, 1.0, 0, phase)
                    sink = blocks.vector_sink_f()
                    item_size = gr.sizeof_float
                src.set_max_output_buffer(4096)
                tb.connect(src, blocks.throttle(item_size, 100000, False),
                           blocks.head(item_size, count), sink)
                tb.start(256)
                try:
                    deadline = time.monotonic() + 5
                    while sink.nitems_read(0) < 4096:
                        self.assertLess(time.monotonic(), deadline)
                        time.sleep(0.001)
                    # The source handles this message between work calls, so
                    # no setter races with sample generation.
                    src._post(pmt.intern('cmd'),
                              pmt.to_pmt({'freq': new_frequency}))
                    tb.wait()
                finally:
                    tb.stop()
                    tb.wait()
                data = sink.data()
                self.assertEqual(len(data), count)
                old_rate = 2 * math.pi * old_frequency / sample_rate
                new_rate = 2 * math.pi * new_frequency / sample_rate
                if complex_output:
                    increments = []
                    for first, second in zip(data, data[1:]):
                        rotation = second * first.conjugate()
                        increments.append(
                            math.atan2(rotation.imag, rotation.real))
                    cut = next(i for i, value in enumerate(increments)
                               if abs(value - new_rate) < 2e-7)
                    for i, value in enumerate(increments):
                        self.assertLess(
                            abs(value - (old_rate if i < cut else new_rate)),
                            2e-7)
                else:
                    cut = next(i for i, value in enumerate(data)
                               if abs(value - data[0]) > 1e-5) - 1
                self.assertGreaterEqual(cut, 4096)
                self.assertLess(cut, count - 512)
                for i, value in enumerate(data):
                    angle = phase + old_rate * min(i, cut) + new_rate * max(
                        0, i - cut)
                    expected = (complex(math.cos(angle), math.sin(angle))
                                if complex_output else math.sin(angle))
                    self.assertLess(abs(value - expected), 6e-6)

    def test_freq_msg(self):  # deprecated but still tested
        src = analog.sig_source_c(8, analog.GR_SIN_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        snk = blocks.vector_sink_c()
        self.tb.connect(src, op, snk)
        self.assertAlmostEqual(src.frequency(), 1.0)

        frequency = 3.0
        src._post(
            pmt.to_pmt('cmd'),
            pmt.to_pmt({
                "freq": frequency
            }))
        self.tb.run()

        self.assertAlmostEqual(src.frequency(), frequency)

    def test_cmd_msg(self):
        src = analog.sig_source_c(8, analog.GR_SIN_WAVE, 1.0, 1.0)
        op = blocks.head(gr.sizeof_gr_complex, 9)
        snk = blocks.vector_sink_c()
        self.tb.connect(src, op, snk)
        self.assertAlmostEqual(src.frequency(), 1.0)

        frequency = 3.0
        amplitude = 10
        offset = -1.0

        src._post(
            pmt.to_pmt('cmd'),
            pmt.to_pmt({
                "freq": frequency,
                "ampl": amplitude,
                "offset": offset
            }))
        self.tb.run()

        self.assertAlmostEqual(src.frequency(), frequency)
        self.assertAlmostEqual(src.amplitude(), amplitude)
        self.assertAlmostEqual(src.offset(), offset)


if __name__ == '__main__':
    gr_unittest.run(test_sig_source)
