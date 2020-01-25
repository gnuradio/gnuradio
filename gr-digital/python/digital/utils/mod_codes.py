#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 

# Constants used to represent what coding to use.
from __future__ import unicode_literals
GRAY_CODE = 'gray'
SET_PARTITION_CODE = 'set-partition'
NO_CODE = 'none'

codes = (GRAY_CODE, SET_PARTITION_CODE, NO_CODE)

def invert_code(code):
    c = enumerate(code)
    ic = [(b, a) for (a, b) in c]
    ic.sort()
    return [a for (b, a) in ic]
