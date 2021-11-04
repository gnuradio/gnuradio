#
# Copyright 2003 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
Display numbers as strings using engineering notation.
"""

scale_factor = {}
scale_factor['E'] = 1e18
scale_factor['P'] = 1e15
scale_factor['T'] = 1e12
scale_factor['G'] = 1e9
scale_factor['M'] = 1e6
scale_factor['k'] = 1e3
scale_factor['m'] = 1e-3
scale_factor['u'] = 1e-6
scale_factor['n'] = 1e-9
scale_factor['p'] = 1e-12
scale_factor['f'] = 1e-15
scale_factor['a'] = 1e-18


def num_to_str(n, precision=6):
    '''Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n'''
    m = abs(n)
    format_spec = '%.' + repr(int(precision)) + 'g'
    if m >= 1e9:
        return '%sG' % float(format_spec % (n * 1e-9))
    elif m >= 1e6:
        return '%sM' % float(format_spec % (n * 1e-6))
    elif m >= 1e3:
        return '%sk' % float(format_spec % (n * 1e-3))
    elif m >= 1:
        return '%s' % float(format_spec % (n))
    elif m >= 1e-3:
        return '%sm' % float(format_spec % (n * 1e3))
    elif m >= 1e-6:
        return '%su' % float(format_spec % (n * 1e6))
    elif m >= 1e-9:
        return '%sn' % float(format_spec % (n * 1e9))
    elif m >= 1e-12:
        return '%sp' % float(format_spec % (n * 1e12))
    elif m >= 1e-15:
        return '%sf' % float(format_spec % (n * 1e15))
    else:
        return '%s' % float(format_spec % (n))


def str_to_num(value):
    '''Convert a string in engineering notation to a number.  E.g., '15m' -> 15e-3'''
    try:
        if not isinstance(value, str):
            raise TypeError("Value must be a string")
        scale = 1.0
        suffix = value[-1]
        if suffix in scale_factor:
            return float(value[0:-1]) * scale_factor[suffix]
        return float(value)
    except (TypeError, KeyError, ValueError):
        raise ValueError(
            "Invalid engineering notation value: %r" % (value,))
