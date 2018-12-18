#!/usr/bin/env python
#
# Copyright 2004,2010,2018 Free Software Foundation, Inc.
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
"""
GNU radio specific extension of unittest.
"""
from __future__ import absolute_import
from __future__ import unicode_literals
from __future__ import division

import os
import stat

import unittest
from . import gr_xmlrunner



class TestCase(unittest.TestCase):
    """A subclass of unittest.TestCase that adds additional assertions

    Adds new methods assertComplexAlmostEqual,
    assertComplexTuplesAlmostEqual and assertFloatTuplesAlmostEqual
    """

    def assertComplexAlmostEqual(self, first, second, places=7, msg=None):
        """Fail if the two complex objects are unequal as determined by their
           difference rounded to the given number of decimal places
           (default 7) and comparing to zero.

           Note that decimal places (from zero) is usually not the same
           as significant digits (measured from the most significant digit).
       """
        if round(second.real-first.real, places) != 0:
            raise self.failureException(
                msg or '%r != %r within %r places' % (first, second, places))
        if round(second.imag-first.imag, places) != 0:
            raise self.failureException(
                msg or '%r != %r within %r places' % (first, second, places)
            )


    def assertComplexAlmostEqual2(self, ref, x, abs_eps=1e-12, rel_eps=1e-6, msg=None):
        """
        Fail if the two complex objects are unequal as determined by both
        absolute delta (abs_eps) and relative delta (rel_eps).
        """
        if abs(ref - x) < abs_eps:
            return

        if abs(ref) > abs_eps:
            if abs(ref-x) / abs(ref) > rel_eps:
                raise self.failureException(
                    msg or '%r != %r rel_error = %r rel_limit = %r' % (
                        ref, x, abs(ref-x) / abs(ref), rel_eps
                    )
                )
        else:
            raise self.failureException(
                msg or '%r != %r rel_error = %r rel_limit = %r' % (
                    ref, x, abs(ref-x) / abs(ref), rel_eps
                )
            )


    def assertComplexTuplesAlmostEqual(self, a, b, places=7, msg=None):
        """
        Fail if the two complex tuples are not approximately equal.
        Approximate equality is determined by specifying the number of decimal
        places.0
        """
        self.assertEqual(len(a), len(b))
        return all((
            self.assertComplexAlmostEqual(x, y, places, msg)
            for (x, y) in zip(a, b)
        ))


    def assertComplexTuplesAlmostEqual2(self, a, b,
                                        abs_eps=1e-12, rel_eps=1e-6, msg=None):
        """
        Fail if the two complex tuples are not approximately equal.
        Approximate equality is determined by calling assertComplexAlmostEqual().
        """
        self.assertEqual(len(a), len(b))
        return all((
            self.assertComplexAlmostEqual2(x, y, abs_eps, rel_eps, msg)
            for (x, y) in zip(a, b)
        ))


    def assertFloatTuplesAlmostEqual(self, a, b, places=7, msg=None):
        """
        Fail if the two real-valued tuples are not approximately equal.
        Approximate equality is determined by specifying the number of decimal
        places.
        """
        self.assertEqual(len(a), len(b))
        return all((
            self.assertAlmostEqual(x, y, places, msg)
            for (x, y) in zip(a, b)
        ))


    def assertFloatTuplesAlmostEqual2(self, a, b,
                                      abs_eps=1e-12, rel_eps=1e-6, msg=None):
        self.assertEqual(len(a), len(b))
        return all((
            self.assertComplexAlmostEqual2(x, y, abs_eps, rel_eps, msg)
            for (x, y) in zip(a, b)
        ))

TestResult = unittest.TestResult
TestSuite = unittest.TestSuite
FunctionTestCase = unittest.FunctionTestCase
TestLoader = unittest.TestLoader
TextTestRunner = unittest.TextTestRunner
TestProgram = unittest.TestProgram
main = TestProgram


def run(PUT, filename=None, verbosity=1):
    '''
    Runs the unittest on a TestCase and produces an optional XML report
    PUT:      the program under test and should be a gr_unittest.TestCase
    filename: an optional filename to save the XML report of the tests
              this will live in ./.unittests/python
    '''
    # Run this is given a file name
    if filename:
        basepath = "./.unittests"
        path = basepath + "/python"

        if not os.path.exists(basepath):
            os.makedirs(basepath, mode=0o750)

        xmlrunner = None
        # only proceed if .unittests is writable
        st = os.stat(basepath)[stat.ST_MODE]
        if st & stat.S_IWUSR > 0:
            # Test if path exists; if not, build it
            if not os.path.exists(path):
                os.makedirs(path, mode=0o750)

            # Just for safety: make sure we can write here, too
            st = os.stat(path)[stat.ST_MODE]
            if st & stat.S_IWUSR > 0:
                # Create an XML runner to filename
                fout = open(path+"/"+filename, "w")
                xmlrunner = gr_xmlrunner.XMLTestRunner(fout)

        # Run the test; runner also creates XML output file
        suite = TestLoader().loadTestsFromTestCase(PUT)

        # use the xmlrunner if we can write the the directory
        if xmlrunner is not None:
            xmlrunner.run(suite)

        main(verbosity=verbosity)
    else:
        # If no filename is given, just run the test
        main(verbosity=verbosity)


##############################################################################
# Executing this module from the command line
##############################################################################

if __name__ == "__main__":
    main(module=None)
