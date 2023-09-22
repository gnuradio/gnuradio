#
# Copyright 2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

class NotDAG (Exception):
    """Not a directed acyclic graph"""
    pass


class CantHappen (Exception):
    """Can't happen"""
    pass
