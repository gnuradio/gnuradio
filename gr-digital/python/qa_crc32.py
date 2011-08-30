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

class test_crc32(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test01 (self):
        data = 100*"0"
        expected_result = 2943744955
        result = digital_swig.crc32(data)
        #print hex(result)
        
        self.assertEqual (expected_result, result)

    def test02 (self):
        data = 100*"1"
        expected_result = 2326594156
        result = digital_swig.crc32(data)
        #print hex(result)
        
        self.assertEqual (expected_result, result)

    def test03 (self):
        data = 10*"0123456789"
        expected_result = 3774345973
        result = digital_swig.crc32(data)
        #print hex(result)

        self.assertEqual (expected_result, result)

if __name__ == '__main__':
    gr_unittest.run(test_crc32, "test_crc32.xml")
