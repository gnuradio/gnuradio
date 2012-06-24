#!/usr/bin/env python
#
# Copyright 2006,2007,2010 Free Software Foundation, Inc.
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

class my_add2_dd(gr.feval_dd):
    def eval(self, x):
        return x + 2

class my_add2_ll(gr.feval_ll):
    def eval(self, x):
        return x + 2

class my_add2_cc(gr.feval_cc):
    def eval(self, x):
        return x + (2 - 2j)

class my_feval(gr.feval):
    def __init__(self):
        gr.feval.__init__(self)
        self.fired = False
    def eval(self):
        self.fired = True

class test_feval(gr_unittest.TestCase):

    def test_dd_1(self):
        f = my_add2_dd()
        src_data =        (0.0, 1.0, 2.0, 3.0, 4.0)
        expected_result = (2.0, 3.0, 4.0, 5.0, 6.0)
        # this is all in python...
        actual_result = tuple([f.eval(x) for x in src_data])
        self.assertEqual(expected_result, actual_result)

    def test_dd_2(self):
        f = my_add2_dd()
        src_data =        (0.0, 1.0, 2.0, 3.0, 4.0)
        expected_result = (2.0, 3.0, 4.0, 5.0, 6.0)
        # this is python -> C++ -> python and back again...
        actual_result = tuple([gr.feval_dd_example(f, x) for x in src_data])
        self.assertEqual(expected_result, actual_result)


    def test_ll_1(self):
        f = my_add2_ll()
        src_data =        (0, 1, 2, 3, 4)
        expected_result = (2, 3, 4, 5, 6)
        # this is all in python...
        actual_result = tuple([f.eval(x) for x in src_data])
        self.assertEqual(expected_result, actual_result)

    def test_ll_2(self):
        f = my_add2_ll()
        src_data =        (0, 1, 2, 3, 4)
        expected_result = (2, 3, 4, 5, 6)
        # this is python -> C++ -> python and back again...
        actual_result = tuple([gr.feval_ll_example(f, x) for x in src_data])
        self.assertEqual(expected_result, actual_result)


    def test_cc_1(self):
        f = my_add2_cc()
        src_data =        (0+1j, 2+3j, 4+5j, 6+7j)
        expected_result = (2-1j, 4+1j, 6+3j, 8+5j)
        # this is all in python...
        actual_result = tuple([f.eval(x) for x in src_data])
        self.assertEqual(expected_result, actual_result)

    def test_cc_2(self):
        f = my_add2_cc()
        src_data =        (0+1j, 2+3j, 4+5j, 6+7j)
        expected_result = (2-1j, 4+1j, 6+3j, 8+5j)
        # this is python -> C++ -> python and back again...
        actual_result = tuple([gr.feval_cc_example(f, x) for x in src_data])
        self.assertEqual(expected_result, actual_result)

    def test_void_1(self):
        # this is all in python
        f = my_feval()
        f.eval()
        self.assertEqual(True, f.fired)

    def test_void_2(self):
        # this is python -> C++ -> python and back again
        f = my_feval()
        gr.feval_example(f)
        self.assertEqual(True, f.fired)


if __name__ == '__main__':
    gr_unittest.run(test_feval, "test_feval.xml")
