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
import pmt_swig as pmt

class test_gruel_pmt(unittest.TestCase):

    def test01 (self):
        a = pmt.pmt_intern("a")
        b = pmt.pmt_from_double(123765)
        d1 = pmt.pmt_make_dict()
        d2 = pmt.pmt_dict_add(d1, a, b)
        pmt.pmt_print(d2)

    def test02 (self):
        const = 123765
        x_pmt = pmt.pmt_from_double(const)
        x_int = pmt.pmt_to_double(x_pmt)
        self.assertEqual(x_int, const)

    def test03(self):
        v = pmt.pmt_init_f32vector(3, [11, -22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test04(self):
        v = pmt.pmt_init_f64vector(3, [11, -22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test05(self):
        v = pmt.pmt_init_u8vector(3, [11, 22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test06(self):
        v = pmt.pmt_init_s8vector(3, [11, -22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test07(self):
        v = pmt.pmt_init_u16vector(3, [11, 22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test08(self):
        v = pmt.pmt_init_s16vector(3, [11, -22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test09(self):
        v = pmt.pmt_init_u32vector(3, [11, 22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test10(self):
        v = pmt.pmt_init_s32vector(3, [11, -22, 33])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test11(self):
        v = pmt.pmt_init_c32vector(3, [11 + -101j, -22 + 202j, 33 + -303j])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

    def test12(self):
        v = pmt.pmt_init_c64vector(3, [11 + -101j, -22 + 202j, 33 + -303j])
        s = pmt.pmt_serialize_str(v)
        d = pmt.pmt_deserialize_str(s)
        self.assertTrue(pmt.pmt_equal(v, d))

if __name__ == '__main__':
    unittest.main()
