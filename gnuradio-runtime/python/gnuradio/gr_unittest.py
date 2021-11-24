#!/usr/bin/env python
#
# Copyright 2004,2010,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
GNU radio specific extension of unittest.
"""

import time
import unittest

# We allow snakeCase here for consistency with unittest
# pylint: disable=invalid-name


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
        if round(second.real - first.real, places) != 0:
            raise self.failureException(
                msg or '%r != %r within %r places' % (first, second, places))
        if round(second.imag - first.imag, places) != 0:
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
            if abs(ref - x) / abs(ref) > rel_eps:
                raise self.failureException(
                    msg or '%r != %r rel_error = %r rel_limit = %r' % (
                        ref, x, abs(ref - x) / abs(ref), rel_eps
                    )
                )
        else:
            raise self.failureException(
                msg or '%r != %r rel_error = %r rel_limit = %r' % (
                    ref, x, abs(ref - x) / abs(ref), rel_eps
                )
            )

    def assertComplexTuplesAlmostEqual(self, a, b, places=7, msg=None):
        """
        Fail if the two complex tuples are not approximately equal.
        Approximate equality is determined by specifying the number of decimal
        places.0
        """
        self.assertEqual(len(a), len(b))
        return all([
            self.assertComplexAlmostEqual(x, y, places, msg)
            for (x, y) in zip(a, b)
        ])

    def assertComplexTuplesAlmostEqual2(self, a, b,
                                        abs_eps=1e-12, rel_eps=1e-6, msg=None):
        """
        Fail if the two complex tuples are not approximately equal.
        Approximate equality is determined by calling assertComplexAlmostEqual().
        """
        self.assertEqual(len(a), len(b))
        return all([
            self.assertComplexAlmostEqual2(x, y, abs_eps, rel_eps, msg)
            for (x, y) in zip(a, b)
        ])

    def assertFloatTuplesAlmostEqual(self, a, b, places=7, msg=None):
        """
        Fail if the two real-valued tuples are not approximately equal.
        Approximate equality is determined by specifying the number of decimal
        places.
        """
        self.assertEqual(len(a), len(b))
        return all([
            self.assertAlmostEqual(x, y, places, msg)
            for (x, y) in zip(a, b)
        ])

    def assertFloatTuplesAlmostEqual2(self, a, b,
                                      abs_eps=1e-12, rel_eps=1e-6, msg=None):
        self.assertEqual(len(a), len(b))
        return all([
            self.assertComplexAlmostEqual2(x, y, abs_eps, rel_eps, msg)
            for (x, y) in zip(a, b)
        ])

    def assertSequenceEqualGR(self, data_in, data_out):
        """
        Note this function exists because of this bug: https://bugs.python.org/issue19217
        Calling self.assertEqual(seqA, seqB) can hang if seqA and seqB are not equal.
        """
        if len(data_in) != len(data_out):
            print(
                'Lengths do not match: {:d} -- {:d}'.format(len(data_in), len(data_out)))
        self.assertTrue(len(data_in) == len(data_out))
        total_miscompares = 0
        for idx, item in enumerate(zip(data_in, data_out)):
            if item[0] != item[1]:
                total_miscompares += 1
                print(
                    'Miscompare at: {:d} ({:d} -- {:d})'.format(idx, item[0], item[1]))
        if total_miscompares > 0:
            print('Total miscompares: {:d}'.format(total_miscompares))
        self.assertTrue(total_miscompares == 0)

    def waitFor(
            self,
            condition,
            timeout=5.0,
            poll_interval=0.2,
            fail_on_timeout=True,
            fail_msg=None):
        """
        Helper function: Wait for a callable to return True within a given
        timeout.

        This is useful for running tests where an exact wait time is not known.

        Arguments:
        - condition: A callable. Must return True when a 'good' condition is met.
        - timeout: Timeout in seconds. `condition` must return True within this
                   timeout.
        - poll_interval: Time between calls to condition() in seconds
        - fail_on_timeout: If True, the test case will fail when the timeout
                           occurs. If False, this function will return False in
                           that case.
        - fail_msg: The message that is printed when a timeout occurs and
                    fail_on_timeout is true.
        """
        if not callable(condition):
            self.fail("Invalid condition provided to waitFor()!")
        stop_time = time.monotonic() + timeout
        while time.monotonic() <= stop_time:
            if condition():
                return True
            time.sleep(poll_interval)
        if fail_on_timeout:
            fail_msg = fail_msg or "Timeout exceeded during call to waitFor()!"
            self.fail(fail_msg)
        return False


TestResult = unittest.TestResult
TestSuite = unittest.TestSuite
FunctionTestCase = unittest.FunctionTestCase
TestLoader = unittest.TestLoader
TextTestRunner = unittest.TextTestRunner
TestProgram = unittest.TestProgram
main = TestProgram


def run(PUT, filename=None, verbosity=1):
    '''
    Runs the unittest on a TestCase
    PUT:      the program under test and should be a gr_unittest.TestCase
    filename: This argument is here for historical reasons.
    '''
    if filename:
        print("DEPRECATED: Using filename with gr_unittest does no longer "
              "have any effect.")
    main(verbosity=verbosity)


##############################################################################
# Executing this module from the command line
##############################################################################

if __name__ == "__main__":
    main(module=None)
