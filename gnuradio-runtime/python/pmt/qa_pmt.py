#!/usr/bin/env python
#
# Copyright 2011, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import unittest
import pmt
import struct


class test_pmt(unittest.TestCase):

    MAXINT32 = (2**31) - 1
    MININT32 = (-MAXINT32) - 1

    def setUp(self):
        from ctypes import sizeof, c_long
        self.sizeof_long = sizeof(c_long)

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
        self.assertEqual(const, pmt.to_long(deser))

    def test15(self):
        if (self.sizeof_long <= 4):
            return
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
        if (self.sizeof_long <= 4):
            return
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
        self.assertTrue(pmt.is_dict(deser))
        self.assertTrue(pmt.equal(d, deser))
        p_dict = pmt.to_python(deser)
        self.assertEqual(self.MAXINT32, p_dict["MAX"])
        self.assertEqual(self.MININT32, p_dict["MIN"])

    def test20_absolute_serialization_misc(self):
        # null SERDES
        in_str = b'\x06'
        out_str = pmt.serialize_str(pmt.PMT_NIL)
        self.assertEqual(out_str, in_str)
        in_str = b'\x07\x06\x06'
        out_pmt = pmt.deserialize_str(in_str)
        self.assertTrue(pmt.equal(out_pmt, pmt.PMT_EOF))

        # T/F SERDES
        in_str = b'\x01'
        out_str = pmt.serialize_str(pmt.PMT_F)
        self.assertEqual(out_str, in_str)
        in_str = b'\x00'
        out_pmt = pmt.deserialize_str(in_str)
        self.assertTrue(pmt.equal(out_pmt, pmt.PMT_T))

        # pair SERDES
        in_pmt = pmt.cons(pmt.intern('string'), pmt.from_long(1000))
        in_str = b'\x07\x02\x00\x06string\x03\x00\x00\x03\xe8'
        out_str = pmt.serialize_str(in_pmt)
        self.assertEqual(out_str, in_str)
        in_str = b'\x07\x07\x06\x06\x07\x00\x01'
        in_pmt = pmt.cons(pmt.PMT_EOF, pmt.cons(pmt.PMT_T, pmt.PMT_F))
        out_pmt = pmt.deserialize_str(in_str)
        self.assertTrue(pmt.equal(out_pmt, in_pmt))

        # dict SERDES
        d = pmt.make_dict()
        d = pmt.dict_add(d, pmt.intern('k0'), pmt.from_long(1))
        d = pmt.dict_add(d, pmt.intern('k1'), pmt.from_double(2.22222))
        d = pmt.dict_add(d, pmt.intern('k2'), pmt.from_long(3))
        in_str = b'\t\x07\x02\x00\x02k2\x03\x00\x00\x00\x03\t\x07\x02\x00\x02k1\x04@\x01\xc7\x1bG\x84#\x10\t\x07\x02\x00\x02k0\x03\x00\x00\x00\x01\x06'
        out_str = pmt.serialize_str(d)
        self.assertEqual(out_str, in_str)
        in_str = b'\t\x07\x02\x00\x03vec\n\x00\x00\x00\x00\x04\x01\x00\x01\x02\x03\x04\x06'
        d = pmt.dict_add(pmt.make_dict(), pmt.intern('vec'),
                         pmt.init_u8vector(4, [1, 2, 3, 4]))
        out_pmt = pmt.deserialize_str(in_str)
        self.assertTrue(pmt.equal(out_pmt, d))

    def test21_absolute_serialization_nums(self):
        # uint64 SERDES
        in_num = 9999876
        in_str = b'\x0b\x00\x00\x00\x00\x00\x98\x96\x04'
        out_str = pmt.serialize_str(pmt.from_uint64(in_num))
        self.assertEqual(out_str, in_str)
        in_str = b'\x0b\xff\xff\xff\xff\xff\xff\xff\xff'
        in_num = 0xffffffffffffffff
        out_num = pmt.to_uint64(pmt.deserialize_str(in_str))
        self.assertEqual(out_num, in_num)

        # long int SERDES
        in_num = 2432141
        in_str = b'\x03\x00%\x1c\x8d'
        out_str = pmt.serialize_str(pmt.from_long(in_num))
        self.assertEqual(out_str, in_str)
        in_str = b'\x03\xfdy\xe4\xb7'
        in_num = -42343241
        out_num = pmt.to_long(pmt.deserialize_str(in_str))
        self.assertEqual(out_num, in_num)

        # float SERDES
        in_num = -1.11
        in_str = b'\x04\xbf\xf1\xc2\x8f`\x00\x00\x00'
        out_str = pmt.serialize_str(pmt.from_float(in_num))
        self.assertEqual(out_str, in_str)
        in_str = b'\x04@\x8e\xdd;`\x00\x00\x00'
        in_num = 987.6539916992188
        out_num = pmt.to_float(pmt.deserialize_str(in_str))
        self.assertEqual(out_num, in_num)

        # double SERDES
        in_num = 987654.321
        in_str = b'\x04A.$\x0c\xa4Z\x1c\xac'
        out_str = pmt.serialize_str(pmt.from_double(in_num))
        self.assertEqual(out_str, in_str)
        in_str = b'\x04\xbf\xdb\x19\x84@A\r\xbc'
        in_num = -.42343241
        out_num = pmt.to_double(pmt.deserialize_str(in_str))
        self.assertEqual(out_num, in_num)

    def test22_absolute_serialization_int_uvecs(self):
        # u8_vector SERDES
        in_vec = [1, 3, 128, 255]
        in_str = b'\n\x00\x00\x00\x00\x04\x01\x00\x01\x03\x80\xff'
        out_str = pmt.serialize_str(pmt.init_u8vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x00\x00\x00\x00\x07\x01\x00\x02\x03\x04\t\n\x19@'
        in_vec = [2, 3, 4, 9, 10, 25, 64]
        out_vec = pmt.u8vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # s8_vector SERDES
        in_vec = [1, 3, 127, -128]
        in_str = b'\n\x01\x00\x00\x00\x04\x01\x00\x01\x03\x7f\x80'
        out_str = pmt.serialize_str(pmt.init_s8vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x01\x00\x00\x00\x07\x01\x00\x02\x00\x04\xf7\n\x19\xc0'
        in_vec = [2, 0, 4, -9, 10, 25, -64]
        out_vec = pmt.s8vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # u16_vector SERDES
        in_vec = [0xfffe, 0, 256, 0xffff]
        in_str = b'\n\x02\x00\x00\x00\x04\x01\x00\xff\xfe\x00\x00\x01\x00\xff\xff'
        out_str = pmt.serialize_str(pmt.init_u16vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x02\x00\x00\x00\x04\x01\x00\xff\xff\x00\x00\x00\x01\x00\x02'
        in_vec = [0xffff, 0, 1, 2]
        out_vec = pmt.u16vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # s16_vector SERDES
        in_vec = [0x7fff, 0, -256, -0x8000]
        in_str = b'\n\x03\x00\x00\x00\x04\x01\x00\x7f\xff\x00\x00\xff\x00\x80\x00'
        out_str = pmt.serialize_str(pmt.init_s16vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x03\x00\x00\x00\x05\x01\x00\x00\x01\x00\x02\x00\x03\x00\x04\xff\xfb'
        in_vec = [1, 2, 3, 4, -5]
        out_vec = pmt.s16vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # u32_vector SERDES
        in_vec = [0x01020304, 0x05060708, 0, 100000000]
        in_str = b'\n\x04\x00\x00\x00\x04\x01\x00\x01\x02\x03\x04\x05\x06\x07\x08\x00\x00\x00\x00\x05\xf5\xe1\x00'
        out_str = pmt.serialize_str(pmt.init_u32vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x04\x00\x00\x00\x06\x01\x00\x00\x00\x000\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\t\x00\x00\x00\x0c\x00\x00\x10\x00'
        in_vec = [48, 0xffffffff, 0, 9, 12, 4096]
        out_vec = pmt.u32vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # s32_vector SERDES
        in_vec = [-0x0, -0x8000000, 1, 0x7ffffff]
        in_str = b'\n\x05\x00\x00\x00\x04\x01\x00\x00\x00\x00\x00\xf8\x00\x00\x00\x00\x00\x00\x01\x07\xff\xff\xff'
        out_str = pmt.serialize_str(pmt.init_s32vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x05\x00\x00\x00\x05\x01\x00\x00\x00\x000\x7f\xff\xff\xff\x00\x00\x00\x00\xff\xff\xff\xf7\xff\xff\xff\xf3'
        in_vec = [48, 0x7fffffff, 0, -9, -13]
        out_vec = pmt.s32vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # u64_vector SERDES NOT EXPOSED

        # i64_vector SERDES NOT EXPOSED

    def test23_absolute_serialization_float_uvecs(self):
        # f32_vector SERDES
        in_vec = [0x01020304, 2.1, 3.1415, 1e-9]
        # old serialization (f32 serialized as f64):
        # in_str = b'\n\x08\x00\x00\x00\x04\x01\x00Ap 0@\x00\x00\x00@\x00\xcc\xcc\xc0\x00\x00\x00@\t!\xca\xc0\x00\x00\x00>\x11.\x0b\xe0\x00\x00\x00'
        in_str = b'\n\x08\x00\x00\x00\x04\x01\x00K\x81\x01\x82@\x06ff@I\x0eV0\x89p_'
        out_str = pmt.serialize_str(pmt.init_f32vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        # old serialization (f32 serialized as f64):
        # in_str = b'\n\x08\x00\x00\x00\x04\x01\x00?\xf0\x00\x00\x00\x00\x00\x00@\x00\x00\x00\x00\x00\x00\x00\xbf\xc0\x00\x00\x00\x00\x00\x00@\xfe$\x00\x00\x00\x00\x00'
        in_str = b'\n\x08\x00\x00\x00\x04\x01\x00?\x80\x00\x00@\x00\x00\x00\xbe\x00\x00\x00G\xf1 \x00'
        in_vec = [1., 2., -0.125, 123456.0]
        out_vec = pmt.f32vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # f64_vector SERDES
        in_vec = [0x010203040506, 2.1, 1e-9]
        in_str = b'\n\t\x00\x00\x00\x03\x01\x00Bp 0@P`\x00@\x00\xcc\xcc\xcc\xcc\xcc\xcd>\x11.\x0b\xe8&\xd6\x95'
        out_str = pmt.serialize_str(pmt.init_f64vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\t\x00\x00\x00\x04\x01\x00?\xf0\x00\x00\x00\x00\x00\x00@\x00\x00\x00\x00\x00\x00\x00\xbf\xc0\x00\x00\x00\x00\x00\x00A\x9do4T\x00\x00\x00'
        in_vec = [1., 2., -0.125, 123456789.0]
        out_vec = pmt.f64vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # c32_vector SERDES
        in_vec = [0x01020304 - 1j, 3.1415 + 99.99j]
        # old serialization (c32 serialized as c64):
        # in_str = b'\n\n\x00\x00\x00\x02\x01\x00Ap 0@\x00\x00\x00\xbf\xf0\x00\x00\x00\x00\x00\x00@\t!\xca\xc0\x00\x00\x00@X\xff\\ \x00\x00\x00'
        in_str = struct.pack(">BBIBBffff", 0x0a, 0x0a, len(in_vec), 1, 0,
                             in_vec[0].real, in_vec[0].imag, in_vec[1].real, in_vec[1].imag)
        out_str = pmt.serialize_str(pmt.init_c32vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_vec = [1 + 1j, .125 - 9999999j]
        # old serialization (c32 serialized as c64):
        # in_str = b'\n\n\x00\x00\x00\x02\x01\x00?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xc0\x00\x00\x00\x00\x00\x00\xc1c\x12\xcf\xe0\x00\x00\x00'
        in_str = struct.pack(">BBIBBffff", 0x0a, 0x0a, len(in_vec), 1, 0,
                             in_vec[0].real, in_vec[0].imag, in_vec[1].real, in_vec[1].imag)
        out_vec = pmt.c32vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

        # c64_vector SERDES
        in_vec = [0xffffffff, .9j]
        in_str = b'\n\x0b\x00\x00\x00\x02\x01\x00A\xef\xff\xff\xff\xe0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00?\xec\xcc\xcc\xcc\xcc\xcc\xcd'
        out_str = pmt.serialize_str(pmt.init_c64vector(len(in_vec), in_vec))
        self.assertEqual(out_str, in_str)
        in_str = b'\n\x0b\x00\x00\x00\x02\x01\x00?\xf0\x00\x00\x00\x00\x00\x00?\xf0\x00\x00\x00\x00\x00\x00?\xc0\x00\x00\x00\x00\x00\x00\xc1c\x12\xcf\xe0\x00\x00\x00'
        in_vec = [1 + 1j, .125 - 9999999j]
        out_vec = pmt.c64vector_elements(pmt.deserialize_str(in_str))
        self.assertEqual(out_vec, in_vec)

    def test23_none_exception(self):
        with self.assertRaises(TypeError):
            pmt.is_bool(None)

        with self.assertRaises(TypeError):
            pmt.is_true(None)

        with self.assertRaises(TypeError):
            pmt.is_false(None)

        with self.assertRaises(TypeError):
            pmt.to_bool(None)

        with self.assertRaises(TypeError):
            pmt.is_symbol(None)

        with self.assertRaises(TypeError):
            pmt.symbol_to_string(None)

        with self.assertRaises(TypeError):
            pmt.is_number(None)

        with self.assertRaises(TypeError):
            pmt.is_integer(None)

        with self.assertRaises(TypeError):
            pmt.to_long(None)

        with self.assertRaises(TypeError):
            pmt.is_uint64(None)

        with self.assertRaises(TypeError):
            pmt.to_uint64(None)

        with self.assertRaises(TypeError):
            pmt.is_real(None)

        with self.assertRaises(TypeError):
            pmt.to_double(None)

        with self.assertRaises(TypeError):
            pmt.to_float(None)

        with self.assertRaises(TypeError):
            pmt.is_complex(None)

        with self.assertRaises(TypeError):
            pmt.to_complex(None)

        with self.assertRaises(TypeError):
            pmt.is_null(None)

        with self.assertRaises(TypeError):
            pmt.is_pair(None)

        with self.assertRaises(TypeError):
            pmt.cons(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.cons(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dcons(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dcons(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.car(None)

        with self.assertRaises(TypeError):
            pmt.cdr(None)

        with self.assertRaises(TypeError):
            pmt.set_car(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.set_car(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.set_cdr(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.set_cdr(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.caar(None)

        with self.assertRaises(TypeError):
            pmt.cadr(None)

        with self.assertRaises(TypeError):
            pmt.cdar(None)

        with self.assertRaises(TypeError):
            pmt.cddr(None)

        with self.assertRaises(TypeError):
            pmt.caddr(None)

        with self.assertRaises(TypeError):
            pmt.cadddr(None)

        with self.assertRaises(TypeError):
            pmt.is_tuple(None)

        for tuple_len in range(1, 11):
            for i in range(tuple_len):
                args = [pmt.PMT_NIL] * tuple_len
                args[i] = None
                with self.assertRaises(TypeError):
                    pmt.make_tuple(*args)

        with self.assertRaises(TypeError):
            pmt.to_tuple(None)

        with self.assertRaises(TypeError):
            pmt.tuple_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.is_vector(None)

        with self.assertRaises(TypeError):
            pmt.make_vector(0, None)

        with self.assertRaises(TypeError):
            pmt.vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.vector_set(None, 0, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.vector_set(pmt.PMT_NIL, 0, None)

        with self.assertRaises(TypeError):
            pmt.vector_fill(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.vector_fill(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.is_blob(None)

        with self.assertRaises(TypeError):
            pmt.blob_data(None)

        with self.assertRaises(TypeError):
            pmt.blob_length(None)

        with self.assertRaises(TypeError):
            pmt.is_uniform_vector(None)

        with self.assertRaises(TypeError):
            pmt.is_u8vector(None)

        with self.assertRaises(TypeError):
            pmt.is_s8vector(None)

        with self.assertRaises(TypeError):
            pmt.is_u16vector(None)

        with self.assertRaises(TypeError):
            pmt.is_s16vector(None)

        with self.assertRaises(TypeError):
            pmt.is_u32vector(None)

        with self.assertRaises(TypeError):
            pmt.is_s32vector(None)

        with self.assertRaises(TypeError):
            pmt.is_u64vector(None)

        with self.assertRaises(TypeError):
            pmt.is_s64vector(None)

        with self.assertRaises(TypeError):
            pmt.is_f32vector(None)

        with self.assertRaises(TypeError):
            pmt.is_f64vector(None)

        with self.assertRaises(TypeError):
            pmt.is_c32vector(None)

        with self.assertRaises(TypeError):
            pmt.is_c64vector(None)

        with self.assertRaises(TypeError):
            pmt.uniform_vector_itemsize(None)

        with self.assertRaises(TypeError):
            pmt.u8vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.s8vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.u16vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.s16vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.u32vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.s32vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.u64vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.s64vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.f32vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.f64vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.c32vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.c64vector_ref(None, 0)

        with self.assertRaises(TypeError):
            pmt.u8vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.s8vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.u16vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.s16vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.u32vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.s32vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.u64vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.s64vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.f32vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.f64vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.c32vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.c64vector_set(None, 0, 0)

        with self.assertRaises(TypeError):
            pmt.uniform_vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u8vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s8vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u16vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s16vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u32vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s32vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u64vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s64vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.f32vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.f64vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.c32vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.c64vector_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u8vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.s8vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.u16vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.s16vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.u32vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.s32vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.u64vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.s64vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.f32vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.f64vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.c32vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.c64vector_elements(None)

        with self.assertRaises(TypeError):
            pmt.uniform_vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u8vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s8vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u16vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s16vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u32vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s32vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.u64vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.s64vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.f32vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.f64vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.c32vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.c64vector_writable_elements(None, 0)

        with self.assertRaises(TypeError):
            pmt.is_dict(None)

        with self.assertRaises(TypeError):
            pmt.dict_add(None, pmt.PMT_NIL, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_add(pmt.PMT_NIL, None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_add(pmt.PMT_NIL, pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dict_delete(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_delete(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dict_has_key(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_has_key(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dict_ref(None, pmt.PMT_NIL, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_ref(pmt.PMT_NIL, None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_ref(pmt.PMT_NIL, pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dict_items(None)

        with self.assertRaises(TypeError):
            pmt.dict_keys(None)

        with self.assertRaises(TypeError):
            pmt.dict_update(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.dict_update(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.dict_values(None)

        with self.assertRaises(TypeError):
            pmt.is_any(None)

        with self.assertRaises(TypeError):
            pmt.is_pdu(None)

        with self.assertRaises(TypeError):
            pmt.eq(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.eq(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.eqv(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.eqv(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.equal(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.equal(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.assq(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.assq(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.assv(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.assv(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.assoc(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.assoc(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.reverse(None)

        with self.assertRaises(TypeError):
            pmt.reverse_x(None)

        with self.assertRaises(TypeError):
            pmt.acons(None, pmt.PMT_NIL, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.acons(pmt.PMT_NIL, None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.acons(pmt.PMT_NIL, pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.nth(0, None)

        with self.assertRaises(TypeError):
            pmt.nthcdr(0, None)

        with self.assertRaises(TypeError):
            pmt.memq(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.memq(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.memv(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.memv(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.member(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.member(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.subsetp(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.subsetp(pmt.PMT_NIL, None)

        for list_len, func in enumerate((pmt.list1, pmt.list2, pmt.list3, pmt.list4, pmt.list5, pmt.list6), start=1):
            for i in range(list_len):
                args = [pmt.PMT_NIL] * list_len
                args[i] = None
                with self.assertRaises(TypeError):
                    func(*args)

        with self.assertRaises(TypeError):
            pmt.list_add(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.list_add(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.list_rm(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.list_rm(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.list_has(None, pmt.PMT_NIL)

        with self.assertRaises(TypeError):
            pmt.list_has(pmt.PMT_NIL, None)

        with self.assertRaises(TypeError):
            pmt.is_eof_object(None)

        with self.assertRaises(TypeError):
            pmt.write_string(None)

        with self.assertRaises(TypeError):
            pmt.print(None)

        with self.assertRaises(TypeError):
            pmt.length(None)

        with self.assertRaises(TypeError):
            pmt.serialize_str(None)


if __name__ == '__main__':
    unittest.main()
