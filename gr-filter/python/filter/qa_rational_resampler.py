#!/usr/bin/env python
#
# Copyright 2005-2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, filter, blocks
import math
import random
import sys


def random_floats(n):
    r = []
    for x in range(n):
        # r.append(float(random.randint(-32768, 32768)))
        r.append(float(random.random()))
    return tuple(r)


def reference_dec_filter(src_data, decim, taps):
    tb = gr.top_block()
    src = blocks.vector_source_f(src_data)
    op = filter.fir_filter_fff(decim, taps)
    dst = blocks.vector_sink_f()
    tb.connect(src, op, dst)
    tb.run()
    result_data = dst.data()
    tb = None
    return result_data


def reference_interp_filter(src_data, interp, taps):
    tb = gr.top_block()
    src = blocks.vector_source_f(src_data)
    op = filter.interp_fir_filter_fff(interp, taps)
    dst = blocks.vector_sink_f()
    tb.connect(src, op, dst)
    tb.run()
    result_data = dst.data()
    tb = None
    return result_data


def reference_interp_dec_filter(src_data, interp, decim, taps):
    tb = gr.top_block()
    src = blocks.vector_source_f(src_data)
    up = filter.interp_fir_filter_fff(interp, (1,))
    dn = filter.fir_filter_fff(decim, taps)
    dst = blocks.vector_sink_f()
    tb.connect(src, up, dn, dst)
    tb.run()
    result_data = dst.data()
    tb = None
    return result_data


class test_rational_resampler (gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)

    def tearDown(self):
        pass

    def test_000_1_to_1(self):
        taps = (-4, 5)
        src_data = (234, -4, 23, -56, 45, 98, -23, -7)
        xr = (1186, -112, 339, -460, -167, 582)
        expected_result = [float(x) for x in xr]

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(1, 1, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_001_interp(self):
        taps = [1, 10, 100, 1000, 10000]
        src_data = (0, 2, 3, 5, 7, 11, 13, 17)
        interpolation = 3
        xr = (2, 20, 200, 2003, 20030,
              300, 3005, 30050,
              500, 5007, 50070,
              700, 7011, 70110,
              1100, 11013, 110130,
              1300, 13017, 130170,
              1700.0, 17000.0, 170000.0, 0.0)
        expected_result = [float(x) for x in xr]

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(interpolation, 1, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_002_interp(self):
        taps = random_floats(31)
        src_data = random_floats(10000)
        interpolation = 3

        expected_result = reference_interp_filter(
            src_data, interpolation, taps)

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(interpolation, 1, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()

        N = 1000
        offset = len(taps) - 1
        self.assertEqual(expected_result[offset:offset + N], result_data[0:N])

    def xtest_003_interp(self):
        taps = random_floats(9)
        src_data = random_floats(10000)
        decimation = 3

        expected_result = reference_dec_filter(src_data, decimation, taps)

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(1, decimation, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()

        N = 10
        offset = 10  # len(taps)-1
        print(expected_result[100 + offset:100 + offset + N])
        print(result_data[100:100 + N])
        #self.assertEqual(expected_result[offset:offset+N], result_data[0:N])

    # FIXME disabled.  Triggers hang on SuSE 10.0
    def xtest_004_decim_random_vals(self):
        MAX_TAPS = 9
        MAX_DECIM = 7
        OUTPUT_LEN = 9

        random.seed(0)    # we want reproducibility

        for ntaps in range(1, MAX_TAPS + 1):
            for decim in range(1, MAX_DECIM + 1):
                for ilen in range(ntaps + decim, ntaps + OUTPUT_LEN * decim):
                    src_data = random_floats(ilen)
                    taps = random_floats(ntaps)
                    expected_result = reference_dec_filter(
                        src_data, decim, taps)

                    tb = gr.top_block()
                    src = blocks.vector_source_f(src_data)
                    op = filter.rational_resampler_fff(1, decim, taps)
                    dst = blocks.vector_sink_f()
                    tb.connect(src, op, dst)
                    tb.run()
                    tb = None
                    result_data = dst.data()

                    L1 = len(result_data)
                    L2 = len(expected_result)
                    L = min(L1, L2)
                    if False:
                        sys.stderr.write(
                            'delta = %2d: ntaps = %d decim = %d ilen = %d\n' %
                            (L2 - L1, ntaps, decim, ilen))
                        sys.stderr.write(
                            '  len(result_data) = %d  len(expected_result) = %d\n' %
                            (len(result_data), len(expected_result)))
                    self.assertEqual(expected_result[0:L], result_data[0:L])

    # FIXME disabled.  Triggers hang on SuSE 10.0

    def xtest_005_interp_random_vals(self):
        MAX_TAPS = 9
        MAX_INTERP = 7
        INPUT_LEN = 9

        random.seed(0)    # we want reproducibility

        for ntaps in range(1, MAX_TAPS + 1):
            for interp in range(1, MAX_INTERP + 1):
                for ilen in range(ntaps, ntaps + INPUT_LEN):
                    src_data = random_floats(ilen)
                    taps = random_floats(ntaps)
                    expected_result = reference_interp_filter(
                        src_data, interp, taps)

                    tb = gr.top_block()
                    src = blocks.vector_source_f(src_data)
                    op = filter.rational_resampler_fff(interp, 1, taps)
                    dst = blocks.vector_sink_f()
                    tb.connect(src, op, dst)
                    tb.run()
                    tb = None
                    result_data = dst.data()
                    L1 = len(result_data)
                    L2 = len(expected_result)
                    L = min(L1, L2)
                    # if True or abs(L1-L2) > 1:
                    if False:
                        sys.stderr.write(
                            'delta = %2d: ntaps = %d interp = %d ilen = %d\n' %
                            (L2 - L1, ntaps, interp, ilen))
                        # sys.stderr.write('  len(result_data) = %d  len(expected_result) = %d\n' %
                        #                 (len(result_data), len(expected_result)))
                    #self.assertEqual(expected_result[0:L], result_data[0:L])
                    # FIXME check first ntaps+1 answers
                    self.assertEqual(
                        expected_result[ntaps + 1:L], result_data[ntaps + 1:L])

    def test_006_interp_decim(self):
        taps = random_floats(31)
        src_data = random_floats(10000)
        interp = 3
        decimation = 2

        expected_result = reference_interp_dec_filter(
            src_data, interp, decimation, taps)

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(interp, decimation, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()

        N = 1000
        offset = len(taps) // 2
        self.assertFloatTuplesAlmostEqual(
            expected_result[offset:offset + N], result_data[0:N], 5)

    def test_007_interp_decim_common_factor(self):
        taps = random_floats(31)
        src_data = random_floats(10000)
        interp = 6
        decimation = 2

        expected_result = reference_interp_dec_filter(
            src_data, interp, decimation, taps)

        tb = gr.top_block()
        src = blocks.vector_source_f(src_data)
        op = filter.rational_resampler_fff(interp, decimation, taps)
        dst = blocks.vector_sink_f()
        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        result_data = dst.data()

        N = 1000
        offset = len(taps) // 2
        self.assertFloatTuplesAlmostEqual(
            expected_result[offset:offset + N], result_data[0:N], 5)


if __name__ == '__main__':
    # FIXME: Disabled, see ticket:210
    gr_unittest.run(test_rational_resampler)
