#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random
import cmath
import time

from gnuradio import gr, gr_unittest, filter, digital, blocks


class test_pfb_clock_sync(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test01(self):
        # Test BPSK sync
        excess_bw = 0.35

        sps = 4
        loop_bw = cmath.pi / 100.0
        nfilts = 32
        init_phase = nfilts / 2
        max_rate_deviation = 0.5
        osps = 1

        ntaps = 11 * int(sps * nfilts)
        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw, ntaps)

        self.test = digital.pfb_clock_sync_ccf(sps, loop_bw, taps,
                                               nfilts, init_phase,
                                               max_rate_deviation,
                                               osps)

        data = 10000 * [complex(1, 0), complex(-1, 0)]
        self.src = blocks.vector_source_c(data, False)

        # pulse shaping interpolation filter
        rrc_taps = filter.firdes.root_raised_cosine(
            nfilts,          # gain
            nfilts,          # sampling rate based on 32 filters in resampler
            1.0,             # symbol rate
            excess_bw,       # excess bandwidth (roll-off factor)
            ntaps)
        self.rrc_filter = filter.pfb_arb_resampler_ccf(sps, rrc_taps)

        self.snk = blocks.vector_sink_c()

        self.tb.connect(self.src, self.rrc_filter, self.test, self.snk)
        self.tb.run()

        expected_result = 10000 * [complex(1, 0), complex(-1, 0)]
        dst_data = self.snk.data()

        # Only compare last Ncmp samples
        Ncmp = 1000
        len_e = len(expected_result)
        len_d = len(dst_data)
        expected_result = expected_result[len_e - Ncmp:]
        dst_data = dst_data[len_d - Ncmp:]

        # for e,d in zip(expected_result, dst_data):
        #    print e, d

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 1)

    def test02(self):
        # Test real BPSK sync
        excess_bw = 0.35

        sps = 4
        loop_bw = cmath.pi / 100.0
        nfilts = 32
        init_phase = nfilts / 2
        max_rate_deviation = 0.5
        osps = 1

        ntaps = 11 * int(sps * nfilts)
        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw, ntaps)

        self.test = digital.pfb_clock_sync_fff(sps, loop_bw, taps,
                                               nfilts, init_phase,
                                               max_rate_deviation,
                                               osps)

        data = 10000 * [1, -1]
        self.src = blocks.vector_source_f(data, False)

        # pulse shaping interpolation filter
        rrc_taps = filter.firdes.root_raised_cosine(
            nfilts,          # gain
            nfilts,          # sampling rate based on 32 filters in resampler
            1.0,             # symbol rate
            excess_bw,       # excess bandwidth (roll-off factor)
            ntaps)
        self.rrc_filter = filter.pfb_arb_resampler_fff(sps, rrc_taps)

        self.snk = blocks.vector_sink_f()

        self.tb.connect(self.src, self.rrc_filter, self.test, self.snk)
        self.tb.run()

        expected_result = 10000 * [1, -1]
        dst_data = self.snk.data()

        # Only compare last Ncmp samples
        Ncmp = 1000
        len_e = len(expected_result)
        len_d = len(dst_data)
        expected_result = expected_result[len_e - Ncmp:]
        dst_data = dst_data[len_d - Ncmp:]

        # for e,d in zip(expected_result, dst_data):
        #    print e, d

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 1)

    def test03(self):
        # Test resting of taps
        excess_bw0 = 0.35
        excess_bw1 = 0.22

        sps = 4
        loop_bw = cmath.pi / 100.0
        nfilts = 32
        init_phase = nfilts / 2
        max_rate_deviation = 0.5
        osps = 1

        ntaps = 11 * int(sps * nfilts)
        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw0, ntaps)

        self.test = digital.pfb_clock_sync_ccf(sps, loop_bw, taps,
                                               nfilts, init_phase,
                                               max_rate_deviation,
                                               osps)

        self.src = blocks.null_source(gr.sizeof_gr_complex)
        self.snk = blocks.null_sink(gr.sizeof_gr_complex)

        self.tb.connect(self.src, self.test, self.snk)
        self.tb.start()
        time.sleep(0.1)

        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw1, ntaps)

        self.test.update_taps(taps)

        self.tb.stop()
        self.tb.wait()

    def test03_f(self):
        # Test resting of taps
        excess_bw0 = 0.35
        excess_bw1 = 0.22

        sps = 4
        loop_bw = cmath.pi / 100.0
        nfilts = 32
        init_phase = nfilts / 2
        max_rate_deviation = 0.5
        osps = 1

        ntaps = 11 * int(sps * nfilts)
        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw0, ntaps)

        self.test = digital.pfb_clock_sync_fff(sps, loop_bw, taps,
                                               nfilts, init_phase,
                                               max_rate_deviation,
                                               osps)

        self.src = blocks.null_source(gr.sizeof_float)
        self.snk = blocks.null_sink(gr.sizeof_float)

        self.tb.connect(self.src, self.test, self.snk)
        self.tb.start()
        time.sleep(0.1)

        taps = filter.firdes.root_raised_cosine(nfilts, nfilts * sps,
                                                1.0, excess_bw1, ntaps)

        self.test.update_taps(taps)

        self.tb.stop()
        self.tb.wait()


if __name__ == '__main__':
    gr_unittest.run(test_pfb_clock_sync)
