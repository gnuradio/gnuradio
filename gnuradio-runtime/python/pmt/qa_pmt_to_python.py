#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import absolute_import


import unittest
import pmt
from pmt import pmt_to_python as pmt2py

class test_pmt_to_python(unittest.TestCase):

    def test_pmt_from_double(self):
        b = pmt.from_double(123765)
        self.assertEqual(pmt.to_python(b), 123765)
        t = pmt.to_pmt(list(range(5)))

    def test_numpy_to_uvector_and_reverse(self):
        import numpy as np
        N = 100
        narr = np.ndarray(N, dtype=np.complex128)
        narr.real[:] = np.random.uniform(size=N)
        narr.imag[:] = np.random.uniform(size=N)
        uvector = pmt2py.numpy_to_uvector(narr)
        nparr = pmt2py.uvector_to_numpy(uvector)
        self.assertTrue(nparr.dtype==narr.dtype)
        self.assertTrue(np.alltrue(nparr == narr))



if __name__ == '__main__':
    unittest.main()
