#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import digital_swig
import random, cmath

class test_mpsk_receiver(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test01 (self):
        # Test BPSK sync
        M = 2
        theta = 0
        loop_bw = cmath.pi/100.0
        fmin = -0.5
        fmax = 0.5
        mu = 0.25
        gain_mu = 0.01
        omega = 2
        gain_omega = 0.001
        omega_rel = 0.001

        self.test = digital_swig.mpsk_receiver_cc(M, theta, loop_bw,
                                                  fmin, fmax, mu, gain_mu,
                                                  omega, gain_omega,
                                                  omega_rel)
        
        data = 1000*[complex(1,0), complex(1,0), complex(-1,0), complex(-1,0)]
        self.src = gr.vector_source_c(data, False)
        self.snk = gr.vector_sink_c()

        self.tb.connect(self.src, self.test, self.snk)
        self.tb.run()
        
        expected_result = 1000*[complex(-0.5,0), complex(0.5,0)]
        dst_data = self.snk.data()

        # Only compare last Ncmp samples
        Ncmp = 100
        len_e = len(expected_result)
        len_d = len(dst_data)
        expected_result = expected_result[len_e - Ncmp:]
        dst_data = dst_data[len_d - Ncmp:]

        #for e,d in zip(expected_result, dst_data):
        #    print e, d
        
        self.assertComplexTuplesAlmostEqual (expected_result, dst_data, 1)


    def test02 (self):
        # Test QPSK sync
        M = 4
        theta = 0
        loop_bw = 2*cmath.pi/100.0
        fmin = -0.5
        fmax = 0.5
        mu = 0.25
        gain_mu = 0.01
        omega = 2
        gain_omega = 0.001
        omega_rel = 0.001

        self.test = digital_swig.mpsk_receiver_cc(M, theta, loop_bw,
                                                  fmin, fmax, mu, gain_mu,
                                                  omega, gain_omega,
                                                  omega_rel)

        data = 1000*[complex( 0.707,  0.707), complex( 0.707,  0.707),
                     complex(-0.707,  0.707), complex(-0.707,  0.707),
                     complex(-0.707, -0.707), complex(-0.707, -0.707),
                     complex( 0.707, -0.707), complex( 0.707, -0.707)]
        self.src = gr.vector_source_c(data, False)
        self.snk = gr.vector_sink_c()

        self.tb.connect(self.src, self.test, self.snk)
        self.tb.run()
        
        expected_result = 1000*[complex(0, -1.0), complex(1.0, 0),
                                complex(0, 1.0), complex(-1.0, 0)]
        dst_data = self.snk.data()

        # Only compare last Ncmp samples
        Ncmp = 100
        len_e = len(expected_result)
        len_d = len(dst_data)
        expected_result = expected_result[len_e - Ncmp:]
        dst_data = dst_data[len_d - Ncmp:]

        #for e,d in zip(expected_result, dst_data):
        #    print e, d

        self.assertComplexTuplesAlmostEqual (expected_result, dst_data, 1)

if __name__ == '__main__':
    gr_unittest.run(test_mpsk_receiver, "test_mpsk_receiver.xml")
