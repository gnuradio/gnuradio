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


class qa_math_functions_complex_float(gr_unittest.TestCase):
    """ Math functions, 'complex<float>' type tests. """

    def setUp(self):
        self.tb = gr.top_block()
        self.in_data = ["-1.0+1j", "-0.5+1j", "0.0+1j", "0.5+1j", "1.0+1j"]


    def _run_function_complex_float(self, function_name, in_custom_data=None):
        tb = self.tb

        python_cmath_func = getattr(cmath, function_name)

        if in_custom_data is not None:
            in_data = in_custom_data
        else:
            in_data = self.in_data

        in_data = list(map(complex, in_data))

        valid_result = []
        for in_number in in_data:
            """
                In C++ template, result is type of complex<float>, but python's
                cmath.* result is double. To get precision more alingned with
                template functions, result will be converted to 'float'.

                However in case of complex numbers, there is no way to 'cast'
                them to float and pass as an imput to cmath.*, that's why
                assertComplexTuplesAlmostEqual2 is used to compare results.
            """
            val_complex_double = python_cmath_func(in_number)

            real_double = val_complex_double.real
            imag_double = val_complex_double.imag

            real_float = struct.unpack("f", struct.pack("f", real_double))[0]
            imag_float = struct.unpack("f", struct.pack("f", imag_double))[0]

            valid_result.append(complex(real_float, imag_float))

        source = blocks.vector_source_c(tuple(in_data), False)
        math_function = getattr(blocks, f"math_functions_{function_name}_complex_float")()
        sink = blocks.vector_sink_c()

        tb.connect(source, math_function)
        tb.connect(math_function, sink)
        tb.run()

        out_data = sink.data()

        # print(in_data)
        # print(valid_result)
        # print(list(out_data))

        self.assertComplexTuplesAlmostEqual2(tuple(valid_result), out_data)

    def tearDown(self):
        self.tb = None

    def test_cos(self):
        self._run_function_complex_float('cos')

    def test_cosh(self):
        self._run_function_complex_float('cosh')

    def test_exp(self):
        self._run_function_complex_float('exp')

    def test_log(self):
        self._run_function_complex_float('log')

    def test_sin(self):
        self._run_function_complex_float('sin')

    def test_sinh(self):
        self._run_function_complex_float('sinh')

    def test_sqrt(self):
        self._run_function_complex_float('sqrt')

    def test_tan(self):
        self._run_function_complex_float('tan')

    def test_tanh(self):
        self._run_function_complex_float('tanh')

    def test_acos(self):
        self._run_function_complex_float('acos')

    def test_acosh(self):
        self._run_function_complex_float('acosh')

    def test_asin(self):
        self._run_function_complex_float('asin')

    def test_asinh(self):
        self._run_function_complex_float('asinh')

    def test_atan(self):
        self._run_function_complex_float('atan', ["-1.0+1j", "-0.5+1j", "0.5+1j", "1.0+1j"])

    def test_atanh(self):
        self._run_function_complex_float('atanh')

if __name__ == '__main__':
    gr_unittest.run(qa_math_functions_complex_float)
