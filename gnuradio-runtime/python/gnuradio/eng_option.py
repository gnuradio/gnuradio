#
# Copyright 2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''Add support for engineering notation to optparse.OptionParser'''

from copy import copy
from optparse import Option, OptionValueError
from . import eng_notation


def check_eng_float(option, opt, value):
    try:
        return eng_notation.str_to_num(value)
    except (ValueError, TypeError):
        raise OptionValueError(
            "option %s: invalid engineering notation value: %r" % (opt, value))


def check_intx(option, opt, value):
    try:
        return int(value, 0)
    except (ValueError, TypeError):
        raise OptionValueError(
            "option %s: invalid integer value: %r" % (opt, value))


class eng_option (Option):
    TYPES = Option.TYPES + ("eng_float", "intx", "subdev")
    TYPE_CHECKER = copy(Option.TYPE_CHECKER)
    TYPE_CHECKER["eng_float"] = check_eng_float
    TYPE_CHECKER["intx"] = check_intx
