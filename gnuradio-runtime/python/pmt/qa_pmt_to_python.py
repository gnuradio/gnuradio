#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
import pmt
import pmt_to_python as pmt2py

class test_pmt_to_python(unittest.TestCase):

    def test_pmt_from_double(self):
        b = pmt.from_double(123765)
        self.assertEqual(pmt.to_python(b), 123765)
        t = pmt.to_pmt(range(5))

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
