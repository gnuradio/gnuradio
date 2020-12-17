#
# Copyright 2020 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# The presence of this file turns this directory into a Python package

'''
This is the GNU Radio NETWORK module. Place your Python package
description here (python/__init__.py).
'''

import os

try:
    from .network_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .network_python import *

# import any pure python here
#
from . import tcp_source
