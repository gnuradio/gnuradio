#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Add support for engineering notation to argparse.ArgumentParser
'''

import argparse
from gnuradio import eng_notation


def intx(string):
    """
    Generic integer type, will interpret string as string literal.
    Does the right thing for 0x1F, 0b10101, 010.
    """
    try:
        return int(string, 0)
    except (ValueError, TypeError):
        raise argparse.ArgumentTypeError(
            "Invalid integer value: {}".format(string)
        )


def eng_float(string):
    """
    Takes a string, returns a float. Accepts engineering notation.
    Designed for use with argparse.ArgumentParser.
    Will raise an ArgumentTypeError if not possible.
    """
    try:
        return eng_notation.str_to_num(string)
    except (TypeError, ValueError):
        raise argparse.ArgumentTypeError(
            "Invalid engineering notation value: {}".format(string)
        )
