#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 


import cmath

from gnuradio import gr, gr_unittest, digital

class test_crc32(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test01(self):
        data = 100*"0"
        expected_result = 2943744955
        result = digital.crc32(data)
        #print hex(result)
        
        self.assertEqual(expected_result, result)

    def test02(self):
        data = 100*"1"
        expected_result = 2326594156
        result = digital.crc32(data)
        #print hex(result)
        
        self.assertEqual(expected_result, result)

    def test03(self):
        data = 10*"0123456789"
        expected_result = 3774345973
        result = digital.crc32(data)
        #print hex(result)

        self.assertEqual(expected_result, result)

if __name__ == '__main__':
    gr_unittest.run(test_crc32, "test_crc32.xml")
