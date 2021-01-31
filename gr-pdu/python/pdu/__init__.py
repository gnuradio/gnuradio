# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks and utilities for PDU based processing.
'''

# The presence of this file turns this directory into a Python package
import os

try:
    from .pdu_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .pdu_python import *

# import any pure python here
