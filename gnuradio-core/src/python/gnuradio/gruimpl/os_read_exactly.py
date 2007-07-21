#
# Copyright 2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
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
