#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import math
import sys


def gen_approx_table(f, nentries, min_x, max_x):
    """return a list of nentries containing tuples of the form:
    (m, c).  min_x and max_x specify the domain
    of the table.
    """
    r = []
    incx = float(max_x - min_x) / nentries
    for i in range(nentries):
        a = (i * incx) + min_x
        b = ((i + 1) * incx) + min_x
        m = (f(b) - f(a)) / (b - a)
        c = f(a)
        r.append((m, c))
    return r


def scaled_sine(x):
    return math.sin(x * math.pi / 2**31)


def gen_sine_table():
    nbits = 10
    nentries = 2**nbits

    # min_x = -2**31
    # max_x =  2**31-1
    min_x = 0
    max_x = 2**32 - 1
    t = gen_approx_table(scaled_sine, nentries, min_x, max_x)

    # sys.stdout.write ('static const int WORDBITS = 32;\n')
    # sys.stdout.write ('static const int NBITS = %d;\n' % (nbits,))

    # sys.stdout.write ('static const double sine_table[%d][2] = {\n'% (nentries,))

    for e in t:
        sys.stdout.write('  { %22.15e, %22.15e },\n' % (e[0], e[1]))

    # sys.stdout.write ('};\n')


if __name__ == '__main__':
    gen_sine_table()
