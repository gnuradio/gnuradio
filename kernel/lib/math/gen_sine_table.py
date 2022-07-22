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
import argparse
import shutil
import os

def parse_args():
    """Parses commandline args."""
    desc='Generate the Sin/Cos lookup tables'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument("--output", default="/tmp/sin_table.h")
    parser.add_argument("--output_copy", default=None)
    return parser.parse_args()

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


def gen_sine_table(args):
    nbits = 10
    nentries = 2**nbits

    # min_x = -2**31
    # max_x =  2**31-1
    min_x = 0
    max_x = 2**32 - 1
    t = gen_approx_table(scaled_sine, nentries, min_x, max_x)

    with open(args.output,'w') as f:
        for e in t:
            f.write('  { %22.15e, %22.15e },\n' % (e[0], e[1]))

    if args.output_copy:
        if not os.path.exists(os.path.dirname(args.output_copy)):
            os.makedirs(os.path.dirname(args.output_copy))
        shutil.copyfile(args.output, args.output_copy)

if __name__ == '__main__':
    args = parse_args()
    gen_sine_table(args)
