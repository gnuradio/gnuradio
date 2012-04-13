#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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

import math
import sys

def wrap (x):
    if x >= 2**31:
        return x - 2**32
    return x

def gen_approx_table (f, nentries, min_x, max_x):
    """return a list of nentries containing tuples of the form:
    (m, c, abs_error).  min_x and max_x specify the domain
    of the table.
    """
    r = []
    incx = float (max_x - min_x) / nentries
    for i in range (nentries):
        a = (i * incx) + min_x
        b = ((i + 1) * incx) + min_x
        m = (f(b)-f(a))/(b-a)
        c = (3*a+b)*(f(a)-f(b))/(4*(b-a)) + (f((a+b)/2) + f(a))/2
        abs_error = c+m*a-f(a)
        r.append ((m, c, abs_error))
    return r

def scaled_sine (x):
    return math.sin (x * math.pi / 2**31)

def gen_sine_table ():
    nbits = 10
    nentries = 2**nbits

    # min_x = -2**31
    # max_x =  2**31-1
    min_x = 0
    max_x = 2**32-1
    t = gen_approx_table (scaled_sine, nentries, min_x, max_x)

    max_error = 0
    for e in t:
        max_error = max (max_error, abs (e[2]))

    # sys.stdout.write ('static const int WORDBITS = 32;\n')
    # sys.stdout.write ('static const int NBITS = %d;\n' % (nbits,))

    sys.stdout.write ('  // max_error = %22.15e\n' % (max_error,))

    # sys.stdout.write ('static const double sine_table[%d][2] = {\n'% (nentries,))

    for e in t:
        sys.stdout.write ('  { %22.15e, %22.15e },\n' % (2 * e[0], e[1]))

    # sys.stdout.write ('};\n')

if __name__ == '__main__':
    gen_sine_table ()
