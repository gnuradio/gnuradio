#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks to connect to audio sources (mic-in) and sinks (speaker-out)
ports on a computer.

The underlying hardware driver is system and OS dependent and this
module should automatically discover the correct one to use.
'''
from __future__ import unicode_literals
import os

try:
    from .audio_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .audio_swig import *
