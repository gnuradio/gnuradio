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
import eng_notation

def check_eng_float (option, opt, value):
    try:
        return eng_notation.str_to_num(value)
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

    Returns:
        a 2-tuple (0|1, 0|1)
    """
    d = { 'A' : (0, 0), 'A:0' : (0, 0), 'A:1' : (0, 1), 'A:2' : (0, 2),
          'B' : (1, 0), 'B:0' : (1, 0), 'B:1' : (1, 1), 'B:2' : (1, 2) }
    try:
        return d[value.upper()]
    except:
        raise OptionValueError(
            "option %s: invalid subdev: '%r', must be one of %s" % (opt, value, ', '.join(sorted(d.keys()))))

class eng_option (Option):
    TYPES = Option.TYPES + ("eng_float", "intx", "subdev")
    TYPE_CHECKER = copy (Option.TYPE_CHECKER)
    TYPE_CHECKER["eng_float"] = check_eng_float
    TYPE_CHECKER["intx"] = check_intx
    TYPE_CHECKER["subdev"] = check_subdev

