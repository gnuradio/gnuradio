from __future__ import unicode_literals
#
# Copyright 2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

def list_reverse(x):
    """
    Return a copy of x that is reverse order.
    """
    r = list(x)
    r.reverse()
    return r

