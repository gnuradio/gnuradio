#!/usr/bin/env python
#
# Copyright 2011,2018 Free Software Foundation, Inc.
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

from __future__ import print_function

import unittest
import pmt

class test_pmt(unittest.TestCase):

    MAXINT32 = (2**31)-1
    MININT32 = (-MAXINT32)-1

    def test01(self):
        a = pmt.intern("a")
        b = pmt.from_double(123765)
        d1 = pmt.make_dict()
        d2 = pmt.dict_add(d1, a, b)
        print(d2)

    def test02(self):
        const = 123765
        x_pmt = pmt.from_double(const)
        x_int = pmt.to_double(x_pmt)
        x_float = pmt.to_float(x_pmt)
        self.assertEqual(x_int, const)
        self.assertEqual(x_float, const)

    def test03(self):
        v = pmt.init_f32vector(3, [11, -22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))
        self.assertEqual(pmt.uniform_vector_itemsize(v), 4)

    def test04(self):
        v = pmt.init_f64vector(3, [11, -22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test05(self):
        v = pmt.init_u8vector(3, [11, 22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test06(self):
        v = pmt.init_s8vector(3, [11, -22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test07(self):
        v = pmt.init_u16vector(3, [11, 22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test08(self):
        v = pmt.init_s16vector(3, [11, -22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test09(self):
        v = pmt.init_u32vector(3, [11, 22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test10(self):
        v = pmt.init_s32vector(3, [11, -22, 33])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test11(self):
        v = pmt.init_c32vector(3, [11 + -101j, -22 + 202j, 33 + -303j])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))
        self.assertEqual(pmt.uniform_vector_itemsize(v), 8)

    def test12(self):
        v = pmt.init_c64vector(3, [11 + -101j, -22 + 202j, 33 + -303j])
        s = pmt.serialize_str(v)
        d = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(v, d))

    def test13(self):
        const = self.MAXINT32
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))
        self.assertEqual(const, pmt.to_long(deser))

    def test14(self):
        const = self.MAXINT32 - 1
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))
        self.assertEqual(const,pmt.to_long(deser))

    def test15(self):
        const = self.MAXINT32 + 1
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))
        x_long = pmt.to_long(deser)
        self.assertEqual(const, x_long)

    def test16(self):
        const = self.MININT32
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))

    def test17(self):
        const = self.MININT32 + 1
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))
        x_long = pmt.to_long(deser)
        self.assertEqual(const, x_long)

    def test18(self):
        const = self.MININT32 - 1
        x_pmt = pmt.from_long(const)
        s = pmt.serialize_str(x_pmt)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(x_pmt, deser))
        x_long = pmt.to_long(deser)
        self.assertEqual(const, x_long)

    def test19(self):
        max_key = pmt.intern("MAX")
        _max = pmt.from_long(self.MAXINT32)
        min_key = pmt.intern("MIN")
        _min = pmt.from_long(self.MININT32)
        d = pmt.make_dict()
        d = pmt.dict_add(d, max_key, _max)
        d = pmt.dict_add(d, min_key, _min)
        s = pmt.serialize_str(d)
        deser = pmt.deserialize_str(s)
        self.assertTrue(pmt.equal(d, deser))
        p_dict = pmt.to_python(deser)
        self.assertEqual(self.MAXINT32, p_dict["MAX"])
        self.assertEqual(self.MININT32, p_dict["MIN"])


if __name__ == '__main__':
    unittest.main()
