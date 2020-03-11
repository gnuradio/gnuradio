from __future__ import division
from __future__ import unicode_literals
#
# Copyright 2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import math

def gcd(a,b):
    while b:
        a,b = b, a % b
    return a

def lcm(a,b):
    return a * b / gcd(a, b)

def log2(x):
    return math.log(x) / math.log(2)
