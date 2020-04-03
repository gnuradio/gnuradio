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
from __future__ import unicode_literals

# import swig generated symbols into the network namespace
from .network_swig import *

# import any pure python here
#
from . import tcp_source
