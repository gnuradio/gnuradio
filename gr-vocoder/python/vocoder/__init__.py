#
# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
This is the gr-vocoder package. This package includes the various
vocoder blocks in GNU Radio.
'''
from __future__ import absolute_import
from __future__ import unicode_literals

import os

try:
    from .vocoder_swig import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "..", "..", "swig"))
    from .vocoder_swig import *

from .cvsd import *
