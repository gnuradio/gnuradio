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
from __future__ import absolute_import
from __future__ import unicode_literals

from copy import copy
from optparse import Option, OptionValueError
from . import eng_notation

def check_eng_float (option, opt, value):
    try:
        return eng_notation.str_to_num(value)
    except (ValueError, TypeError):
        raise OptionValueError (
            "option %s: invalid engineering notation value: %r" % (opt, value))

def check_intx (option, opt, value):
    try:
        return int (value, 0)
    except (ValueError, TypeError):
        raise OptionValueError (
            "option %s: invalid integer value: %r" % (opt, value))

class eng_option (Option):
    TYPES = Option.TYPES + ("eng_float", "intx", "subdev")
    TYPE_CHECKER = copy (Option.TYPE_CHECKER)
    TYPE_CHECKER["eng_float"] = check_eng_float
    TYPE_CHECKER["intx"] = check_intx

