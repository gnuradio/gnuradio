# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


try:
    from collections import ChainMap
except ImportError:
    from .chainmap import ChainMap
