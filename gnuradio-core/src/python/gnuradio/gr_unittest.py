#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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
import sys

class TestCase(unittest.TestCase):
    """A subclass of unittest.TestCase that adds additional assertions

    Adds new methods assertComplexAlmostEqual,
    assertComplexTuplesAlmostEqual and assertFloatTuplesAlmostEqual
    """

    def assertComplexAlmostEqual (self, first, second, places=7, msg=None):
        """Fail if the two complex objects are unequal as determined by their
           difference rounded to the given number of decimal places
           (default 7) and comparing to zero.

           Note that decimal places (from zero) is usually not the same
           as significant digits (measured from the most signficant digit).
        """
        if round(second.real-first.real, places) != 0:
            raise self.failureException, \
                  (msg or '%s != %s within %s places' % (`first`, `second`, `places` ))
        if round(second.imag-first.imag, places) != 0:
            raise self.failureException, \
                  (msg or '%s != %s within %s places' % (`first`, `second`, `places` ))
        
    def assertComplexAlmostEqual2 (self, ref, x, abs_eps=1e-12, rel_eps=1e-6, msg=None):
        """
        Fail if the two complex objects are unequal as determined by...
        """
        if abs(ref - x) < abs_eps:
            return

        if abs(ref) > abs_eps:
            if abs(ref-x)/abs(ref) > rel_eps:
                raise self.failureException, \
                      (msg or '%s != %s rel_error = %s rel_limit = %s' % (
                    `ref`, `x`, abs(ref-x)/abs(ref), `rel_eps` ))
        else:
            raise self.failureException, \
                      (msg or '%s != %s rel_error = %s rel_limit = %s' % (
                    `ref`, `x`, abs(ref-x)/abs(ref), `rel_eps` ))


        
    def assertComplexTuplesAlmostEqual (self, a, b, places=7, msg=None):
        self.assertEqual (len(a), len(b))
        for i in xrange (len(a)):
            self.assertComplexAlmostEqual (a[i], b[i], places, msg)

    def assertComplexTuplesAlmostEqual2 (self, ref, x,
                                         abs_eps=1e-12, rel_eps=1e-6, msg=None):
        self.assertEqual (len(ref), len(x))
        for i in xrange (len(ref)):
            try:
                self.assertComplexAlmostEqual2 (ref[i], x[i], abs_eps, rel_eps, msg)
            except self.failureException, e:
                #sys.stderr.write("index = %d " % (i,))
                #sys.stderr.write("%s\n" % (e,))
                raise

    def assertFloatTuplesAlmostEqual (self, a, b, places=7, msg=None):
        self.assertEqual (len(a), len(b))
        for i in xrange (len(a)):
            self.assertAlmostEqual (a[i], b[i], places, msg)


    def assertFloatTuplesAlmostEqual2 (self, ref, x,
                                       abs_eps=1e-12, rel_eps=1e-6, msg=None):
        self.assertEqual (len(ref), len(x))
        for i in xrange (len(ref)):
            try:
                self.assertComplexAlmostEqual2 (ref[i], x[i], abs_eps, rel_eps, msg)
            except self.failureException, e:
                #sys.stderr.write("index = %d " % (i,))
                #sys.stderr.write("%s\n" % (e,))
                raise


TestResult = unittest.TestResult
TestSuite = unittest.TestSuite
FunctionTestCase = unittest.FunctionTestCase
TestLoader = unittest.TestLoader
TextTestRunner = unittest.TextTestRunner
TestProgram = unittest.TestProgram
main = TestProgram

##############################################################################
# Executing this module from the command line
##############################################################################

if __name__ == "__main__":
    main(module=None)
