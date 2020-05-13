#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 gnuradio.org.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import math
import cmath
import struct


class qa_math_functions_float(gr_unittest.TestCase):
    """ Math functions, 'float' type tests. """

    def setUp(self):
        self.tb = gr.top_block()
        self.in_data = [-1.0, -0.5, 0.0, 0.5, 1.0]

    def _run_function_float(self, function_name, in_custom_data=None):
        tb = self.tb

        python_math_func = getattr(math, function_name)

        if in_custom_data is not None:
            in_data = in_custom_data
        else:
            in_data = self.in_data

        valid_result = []
        for in_number in in_data:
            """
                In C++ template, result 'double' is cast to 'float', hence number
                precision will change. Same goes here, to get precision more alingned
                with template functions, python's math.* functoins will be converted
                to 'float'.

                Same for the math.* argument.
            """
            in_double_to_float = struct.unpack("f", struct.pack("f", in_number))[0]

            val_double = python_math_func(in_double_to_float)

            val_double_to_float = struct.unpack("f", struct.pack("f", val_double))[0]

            valid_result.append(val_double_to_float)

        source = blocks.vector_source_f(tuple(in_data), False)
        math_function = getattr(blocks, f"math_functions_{function_name}_float")()
        sink = blocks.vector_sink_f()

        tb.connect(source, math_function)
        tb.connect(math_function, sink)
        tb.run()

        out_data = sink.data()

        # print(in_data)
        # print(valid_result)
        # print(list(out_data))

        self.assertEqual(tuple(valid_result), out_data)

    def tearDown(self):
        self.tb = None

    def test_cos(self):
        self._run_function_float('cos')

    def test_cosh(self):
        self._run_function_float('cosh')

    def test_exp(self):
        self._run_function_float('exp')

    def test_log(self):
        self._run_function_float('log', [0.5, 1.0, 1.5, 2.0, 2.5])

    def test_sin(self):
        self._run_function_float('sin')

    def test_sinh(self):
        self._run_function_float('sinh')

    def test_sqrt(self):
        self._run_function_float('sqrt', [0.5, 1.0, 1.5, 2.0, 2.5])

    def test_tan(self):
        self._run_function_float('tan')

    def test_tanh(self):
        self._run_function_float('tanh')

    def test_acos(self):
        self._run_function_float('acos')

    def test_acosh(self):
        self._run_function_float('acosh', [1.0, 1.5, 2.0, 2.5])

    def test_asin(self):
        self._run_function_float('asin')

    def test_asinh(self):
        self._run_function_float('asinh')

    def test_atan(self):
        self._run_function_float('atan')

    def test_atanh(self):
        self._run_function_float('atanh', [-0.3, -0.2, -0.1, 0.1, 0.2, 0.3])

if __name__ == '__main__':
    gr_unittest.run(qa_math_functions_float)
