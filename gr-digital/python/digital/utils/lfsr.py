#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 

def lfsr_args(seed, *exp):
    """
    Produce arguments to create scrambler objects from exponent polynomial expressions.
     seed: start-value of register
    *exp: exponents of desired polynomial.
     Example:
    >>> l = digital.lfsr(*lfrs_args(0b11001,7,1,0))
    Creates an lfsr object with seed 0b11001, mask 0b1000011, K=6
    """
    from functools import reduce
    return reduce(int.__xor__, map(lambda x:2**x, exp)), seed, max(exp)-1
