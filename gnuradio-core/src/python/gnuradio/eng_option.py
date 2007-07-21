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

'''Add support for engineering notation to optparse.OptionParser'''

from copy import copy
from optparse import Option, OptionValueError

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


def check_eng_float (option, opt, value):
    try:
        scale = 1.0
        suffix = value[-1]
        if scale_factor.has_key (suffix):
            return float (value[0:-1]) * scale_factor[suffix]
        return float (value)
    except:
        raise OptionValueError (
            "option %s: invalid engineering notation value: %r" % (opt, value))

def check_intx (option, opt, value):
    try:
        return int (value, 0)
    except:
        raise OptionValueError (
            "option %s: invalid integer value: %r" % (opt, value))

def check_subdev (option, opt, value):
    """
    Value has the form: (A|B)(:0|1)?

    @returns a 2-tuple (0|1, 0|1)
    """
    d = { 'A'   : (0, 0),  'A:0' : (0, 0),  'A:1' : (0, 1),
          'B'   : (1, 0),  'B:0' : (1, 0),  'B:1' : (1, 1) }
    try:
        return d[value.upper()]
    except:
        raise OptionValueError(
            "option %s: invalid subdev: '%r', must be one of A, B, A:0, A:1, B:0, B:1" % (opt, value))

class eng_option (Option):
    TYPES = Option.TYPES + ("eng_float", "intx", "subdev")
    TYPE_CHECKER = copy (Option.TYPE_CHECKER)
    TYPE_CHECKER["eng_float"] = check_eng_float
    TYPE_CHECKER["intx"] = check_intx
    TYPE_CHECKER["subdev"] = check_subdev

