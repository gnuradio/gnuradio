"""Utilities that interact and query the system.

Copyright 2008-2011,2015 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-3.0-or-later
"""

import os
from sys import platform


def get_modifier_key(angle_brackets=False):
    """Get the modifier key based on platform.

    Args:
        angle_brackets: if return the modifier key with <> or not

    Returns:
        return the string with the modifier key
    """
    if platform == "darwin":
        if angle_brackets:
            return "<Meta>"
        else:
            return "Meta"
    else:
        if angle_brackets:
            return "<Ctrl>"
        else:
            return "Ctrl"


_nproc = None


def get_cmake_nproc():
    """ Get number of cmake processes for C++ flowgraphs """
    global _nproc  # Cached result
    if _nproc:
        return _nproc
    try:
        # See https://docs.python.org/3.8/library/os.html#os.cpu_count
        _nproc = len(os.sched_getaffinity(0))
    except:
        _nproc = os.cpu_count()
    if not _nproc:
        _nproc = 1

    _nproc = max(_nproc // 2 - 1, 1)
    return _nproc
