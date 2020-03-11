from __future__ import unicode_literals
#
# Copyright 2005 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import os

def os_read_exactly(file_descriptor, nbytes):
    """
    Replacement for os.read that blocks until it reads exactly nbytes.

    """
    s = ''
    while nbytes > 0:
        sbuf = os.read(file_descriptor, nbytes)
        if not(sbuf):
            return ''
        nbytes -= len(sbuf)
        s = s + sbuf
    return s
