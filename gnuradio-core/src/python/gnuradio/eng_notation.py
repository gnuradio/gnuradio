#
# Copyright 2003 Free Software Foundation, Inc.
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

scale_factor = {}
scale_factor['E'] = 1e18
scale_factor['P'] = 1e15
scale_factor['T'] = 1e12
scale_factor['G'] = 1e9
scale_factor['M'] = 1e6
scale_factor['k'] = 1e3
scale_factor['m'] = 1e-3
scale_factor['u'] = 1e-6
scale_factor['n'] = 1e-9
scale_factor['p'] = 1e-12
scale_factor['f'] = 1e-15
scale_factor['a'] = 1e-18

def num_to_str (n):
    '''Convert a number to a string in engineering notation.  E.g., 5e-9 -> 5n'''
    m = abs(n)
    if m >= 1e9:
        return "%gG" % (n * 1e-9)
    elif m >= 1e6:
        return "%gM" % (n * 1e-6)
    elif m >= 1e3:
        return "%gk" % (n * 1e-3)
    elif m >= 1:
        return "%g" % (n)
    elif m >= 1e-3:
        return "%gm" % (n * 1e3)
    elif m >= 1e-6:
        return "%gu" % (n * 1e6)        # where's that mu when you need it (unicode?)
    elif m >= 1e-9:
        return "%gn" % (n * 1e9)
    elif m >= 1e-12:
        return "%gp" % (n * 1e12)
    elif m >= 1e-15:
        return "%gf" % (n * 1e15)
    else:
        return "%g" % (n)


def str_to_num (value):
    '''Convert a string in engineering notation to a number.  E.g., '15m' -> 15e-3'''
    try:
        scale = 1.0
        suffix = value[-1]
        if scale_factor.has_key (suffix):
            return float (value[0:-1]) * scale_factor[suffix]
        return float (value)
    except:
        raise RuntimeError (
            "Invalid engineering notation value: %r" % (value,))
