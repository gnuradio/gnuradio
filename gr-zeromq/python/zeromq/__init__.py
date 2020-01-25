#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks for interfacing with ZeroMQ endpoints.
'''
from __future__ import absolute_import
from __future__ import unicode_literals

import os

try:
    from .zeromq_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .zeromq_swig import *

from .probe_manager import probe_manager
from .rpc_manager import rpc_manager
