#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later

from gnuradio import gr, gr_unittest, blocks
from gnuradio import soapy

class test_soapy_types(gr_unittest.TestCase):

    def test_range(self):
        test_range = soapy.range_t()
        self.assertAlmostEqual(test_range.minimum(), 0.0, 9)
        self.assertAlmostEqual(test_range.maximum(), 0.0, 9)
        # Don't check step, uninitialized in old Soapy versions

        test_range = soapy.range_t(-1.23456, 1.23456)
        self.assertAlmostEqual(test_range.minimum(), -1.23456, 9)
        self.assertAlmostEqual(test_range.maximum(), 1.23456, 9)
        self.assertAlmostEqual(test_range.step(), 0.0, 9)

        test_range = soapy.range_t(-7.89012, 7.89012, 0.2468)
        self.assertAlmostEqual(test_range.minimum(), -7.89012, 9)
        self.assertAlmostEqual(test_range.maximum(), 7.89012, 9)
        self.assertAlmostEqual(test_range.step(), 0.2468, 9)

    def test_arginfo(self):
        test_arginfo = soapy.arginfo_t()

        test_arginfo.key = "testkey"
        self.assertEqual(test_arginfo.key, "testkey")

        test_arginfo.name = "testname"
        self.assertEqual(test_arginfo.name, "testname")

        test_arginfo.description = "testdescription"
        self.assertEqual(test_arginfo.description, "testdescription")

        test_arginfo.units = "testunits"
        self.assertEqual(test_arginfo.units, "testunits")

        for val in [soapy.argtype_t.BOOL, soapy.argtype_t.INT, soapy.argtype_t.FLOAT, soapy.argtype_t.STRING]:
            test_arginfo.type = val
            self.assertEqual(test_arginfo.type, val)

        test_arginfo.range = soapy.range_t(1,2,0.5)
        self.assertAlmostEqual(test_arginfo.range.minimum(), 1, 9)
        self.assertAlmostEqual(test_arginfo.range.maximum(), 2, 9)
        self.assertAlmostEqual(test_arginfo.range.step(), 0.5, 9)

        test_arginfo.options = ["opt1", "opt2", "opt3"]
        self.assertEqual(len(test_arginfo.options), 3)
        self.assertEqual(test_arginfo.options[0], "opt1")
        self.assertEqual(test_arginfo.options[1], "opt2")
        self.assertEqual(test_arginfo.options[2], "opt3")

        test_arginfo.option_names = ["Option1", "Option2", "Option3"]
        self.assertEqual(len(test_arginfo.option_names), 3)
        self.assertEqual(test_arginfo.option_names[0], "Option1")
        self.assertEqual(test_arginfo.option_names[1], "Option2")
        self.assertEqual(test_arginfo.option_names[2], "Option3")

        #
        # Test all value types
        #

        test_arginfo.value = "testvalue"
        self.assertEqual(test_arginfo.type, soapy.argtype_t.STRING)
        self.assertEqual(type(test_arginfo.value), str)
        self.assertEqual(test_arginfo.value, "testvalue")

        test_arginfo.value = False
        self.assertEqual(test_arginfo.type, soapy.argtype_t.BOOL)
        self.assertEqual(type(test_arginfo.value), bool)
        self.assertFalse(test_arginfo.value)

        test_arginfo.value = 100
        self.assertEqual(test_arginfo.type, soapy.argtype_t.INT)
        self.assertEqual(type(test_arginfo.value), int)
        self.assertEqual(test_arginfo.value, 100)

        test_arginfo.value = 1.23
        self.assertEqual(test_arginfo.type, soapy.argtype_t.FLOAT)
        self.assertEqual(type(test_arginfo.value), float)
        self.assertAlmostEqual(test_arginfo.value, 1.23, 6)


if __name__ == '__main__':
    gr_unittest.run(test_soapy_types)
