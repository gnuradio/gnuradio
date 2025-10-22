#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import unittest
import pmt
from pmt import pmt_to_python as pmt2py
import numpy as np


class test_pmt_to_python(unittest.TestCase):

    def test_pmt_from_double(self):
        b = pmt.from_double(123765)
        self.assertEqual(pmt.to_python(b), 123765)
        t = pmt.to_pmt(list(range(5)))
        self.assertTrue(pmt.is_vector(t))

    def test_numpy_to_uvector_and_reverse(self):
        N = 100
        narr = np.ndarray(N, dtype=np.cdouble)
        narr.real[:] = np.random.uniform(size=N)
        narr.imag[:] = np.random.uniform(size=N)
        uvector = pmt2py.numpy_to_uvector(narr)
        nparr = pmt2py.uvector_to_numpy(uvector)
        self.assertEqual(nparr.dtype, narr.dtype)
        self.assertTrue(np.all(nparr == narr))

    def test_python_to_pair(self):
        pdu = pmt.to_pair({"key": 42}, np.arange(64, dtype=np.uint8))
        self.assertTrue(pmt.is_pdu(pdu))
        self.assertTrue(pmt.is_pair(pdu))
        metadict = pmt.car(pdu)
        self.assertTrue(pmt.is_dict(metadict))
        val = pmt.dict_ref(metadict, pmt.intern("key"), not_found=pmt.from_long(0))
        self.assertEqual(42, pmt.to_long(val))
        uvec = pmt.cdr(pdu)
        self.assertTrue(pmt.is_u8vector(uvec))
        self.assertListEqual(list(range(64)), pmt.u8vector_elements(uvec))


if __name__ == '__main__':
    unittest.main()
