# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

'''
Blocks and utilities for Video SDL module
'''

# The presence of this file turns this directory into a Python package
import os

try:
    from .video_sdl_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .video_sdl_python import *
