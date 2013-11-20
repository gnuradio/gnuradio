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

import random
import cmath
import time

from gnuradio import gr, gr_unittest, digital, filter, blocks

class test_mpsk_receiver(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test01(self):
        # Test BPSK sync
        M = 2
        theta = 0
        loop_bw = cmath.pi/100.0
        fmin = -0.5
        fmax = 0.5
        mu = 0.5
        gain_mu = 0.01
        omega = 2
        gain_omega = 0.001
        omega_rel = 0.001

        self.test = digital.mpsk_receiver_cc(M, theta, loop_bw,
                                             fmin, fmax, mu, gain_mu,
                                             omega, gain_omega,
                                             omega_rel)
        
        data = 10000*[complex(1,0), complex(-1,0)]
        #data = [2*random.randint(0,1)-1 for x in xrange(10000)]
        self.src = blocks.vector_source_c(data, False)
        self.snk = blocks.vector_sink_c()

        # pulse shaping interpolation filter
        nfilts = 32
        excess_bw = 0.35
        ntaps = 11 * int(omega*nfilts)
        rrc_taps0 = filter.firdes.root_raised_cosine(
            nfilts, nfilts, 1.0, excess_bw, ntaps)
        rrc_taps1 = filter.firdes.root_raised_cosine(
            1, omega, 1.0, excess_bw, 11*omega)
        self.rrc0 = filter.pfb_arb_resampler_ccf(omega, rrc_taps0)
        self.rrc1 = filter.fir_filter_ccf(1, rrc_taps1)

        self.tb.connect(self.src, self.rrc0, self.rrc1, self.test, self.snk)
        self.tb.run()
        
        expected_result = [-0.5*d for d in data]
        dst_data = self.snk.data()

        # Only Ncmp samples after Nstrt samples
        Nstrt = 9000
        Ncmp = 1000
        expected_result = expected_result[Nstrt:Nstrt+Ncmp]
        dst_data = dst_data[Nstrt:Nstrt+Ncmp]
        
        #for e,d in zip(expected_result, dst_data):
        #    print "{0:+.02f}  {1:+.02f}".format(e, d)
        
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 1)


    def test02(self):
        # Test QPSK sync
        M = 4
        theta = 0
        loop_bw = cmath.pi/100.0
        fmin = -0.5
        fmax = 0.5
        mu = 0.5
        gain_mu = 0.01
        omega = 2
        gain_omega = 0.001
        omega_rel = 0.001

        self.test = digital.mpsk_receiver_cc(M, theta, loop_bw,
                                             fmin, fmax, mu, gain_mu,
                                             omega, gain_omega,
                                             omega_rel)

        data = 10000*[complex( 0.707,  0.707),
                     complex(-0.707,  0.707), 
                     complex(-0.707, -0.707), 
                     complex( 0.707, -0.707)]
        data = [0.5*d for d in data]
        self.src = blocks.vector_source_c(data, False)
        self.snk = blocks.vector_sink_c()

        # pulse shaping interpolation filter
        nfilts = 32
        excess_bw = 0.35
        ntaps = 11 * int(omega*nfilts)
        rrc_taps0 = filter.firdes.root_raised_cosine(
            nfilts, nfilts, 1.0, excess_bw, ntaps)
        rrc_taps1 = filter.firdes.root_raised_cosine(
            1, omega, 1.0, excess_bw, 11*omega)
        self.rrc0 = filter.pfb_arb_resampler_ccf(omega, rrc_taps0)
        self.rrc1 = filter.fir_filter_ccf(1, rrc_taps1)

        self.tb.connect(self.src, self.rrc0, self.rrc1, self.test, self.snk)
        self.tb.run()
        
        expected_result = 10000*[complex(-0.5, +0.0), complex(+0.0, -0.5),
                                 complex(+0.5, +0.0), complex(+0.0, +0.5)]

        dst_data = self.snk.data()

        # Only Ncmp samples after Nstrt samples
        Nstrt = 9000
        Ncmp = 1000
        expected_result = expected_result[Nstrt:Nstrt+Ncmp]
        dst_data = dst_data[Nstrt:Nstrt+Ncmp]

        #for e,d in zip(expected_result, dst_data):
        #    print "{0:+.02f}  {1:+.02f}".format(e, d)

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 1)

if __name__ == '__main__':
    gr_unittest.run(test_mpsk_receiver, "test_mpsk_receiver.xml")
