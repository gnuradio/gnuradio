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

import unittest
import pmt_swig

class test_gruel_pmt(unittest.TestCase):

    def test01 (self):
        a = pmt_swig.pmt_intern("a")
        b = pmt_swig.pmt_from_double(123765)
        d1 = pmt_swig.pmt_make_dict()
        d2 = pmt_swig.pmt_dict_add(d1, a, b)
        pmt_swig.pmt_print(d2)

    def test02 (self):
        const = 123765
        x_pmt = pmt_swig.pmt_from_double(const)
        x_int = pmt_swig.pmt_to_double(x_pmt)
        self.assertEqual(x_int, const)

if __name__ == '__main__':
    unittest.main()
