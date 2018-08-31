#!/usr/bin/env python
#
# Copyright 2011-2013,2018 Free Software Foundation, Inc.
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
""" Test digital.mpsk_snr_est_cc """

import random

from gnuradio import gr, gr_unittest, digital, blocks

def random_bit():
    """Create random bits using random() rather than randint(). The latter
    changed for Python 3.2."""
    return random.random() > .5
def get_cplx():
    "Return a BPSK symbol (complex)"
    return complex(2*random_bit() - 1, 0)
def get_n_cplx():
    "Return random, normal-distributed complex number"
    return complex(random.random()-0.5, random.random()-0.5)

class test_mpsk_snr_est(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        random.seed(0) # make repeatable
        N = 10000
        self._noise = [get_n_cplx() for _ in range(N)]
        self._bits = [get_cplx() for _ in range(N)]

    def tearDown(self):
        self.tb = None

    def mpsk_snr_est_setup(self, op):
        result = []
        for i in range(1, 6):
            src_data = [b+(i*n) for b,n in zip(self._bits, self._noise)]
            src = blocks.vector_source_c(src_data)
            dst = blocks.null_sink(gr.sizeof_gr_complex)
            tb = gr.top_block()
            tb.connect(src, op)
            tb.connect(op, dst)
            tb.run() # run the graph and wait for it to finish

            result.append(op.snr())
        return result

    def test_mpsk_snr_est_simple(self):
        expected_result = [8.20, 4.99, 3.23, 2.01, 1.03]

        N = 10000
        alpha = 0.001
        op = digital.mpsk_snr_est_cc(digital.SNR_EST_SIMPLE, N, alpha)

        actual_result = self.mpsk_snr_est_setup(op)
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 2)

    def test_mpsk_snr_est_skew(self):
        expected_result = [8.31, 1.83, -1.68, -3.56, -4.68]

        N = 10000
        alpha = 0.001
        op = digital.mpsk_snr_est_cc(digital.SNR_EST_SKEW, N, alpha)

        actual_result = self.mpsk_snr_est_setup(op)
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 2)

    def test_mpsk_snr_est_m2m4(self):
        expected_result = [8.01, 3.19, 1.97, 2.15, 2.65]

        N = 10000
        alpha = 0.001
        op = digital.mpsk_snr_est_cc(digital.SNR_EST_M2M4, N, alpha)

        actual_result = self.mpsk_snr_est_setup(op)
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 2)

    def test_mpsk_snr_est_svn(self):
        expected_result = [7.91, 3.01, 1.77, 1.97, 2.49]

        N = 10000
        alpha = 0.001
        op = digital.mpsk_snr_est_cc(digital.SNR_EST_SVR, N, alpha)

        actual_result = self.mpsk_snr_est_setup(op)
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 2)

    def test_probe_mpsk_snr_est_m2m4(self):
        expected_result = [8.01, 3.19, 1.97, 2.15, 2.65]

        actual_result = []
        for i in range(1,6):
            src_data = [b+(i*n) for b,n in zip(self._bits, self._noise)]

            src = blocks.vector_source_c(src_data)

            N = 10000
            alpha = 0.001
            op = digital.probe_mpsk_snr_est_c(digital.SNR_EST_M2M4, N, alpha)

            tb = gr.top_block()
            tb.connect(src, op)
            tb.run()               # run the graph and wait for it to finish

            actual_result.append(op.snr())
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 2)

if __name__ == '__main__':
    # Test various SNR estimators; we're not using a Gaussian
    # noise source, so these estimates have no real meaning;
    # just a sanity check.
    gr_unittest.run(test_mpsk_snr_est, "test_mpsk_snr_est.xml")
