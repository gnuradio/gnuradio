#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

"""
Reads output from run_synthetic.py and runs gnuplot showing
GFLOPS as f(npipes, nstages)
"""

import re
import sys
import os
import tempfile
from argparse import ArgumentParser


def parse_file(input_filename, output):
    last = None
    desc = ''
    for line in open(input_filename, 'r'):
        s = line.strip()
        if s.startswith('>>>'):         # ignore ">>> using SSE cruft"
            continue

        if s.startswith('#D'):          # machine description
            desc = s[2:].strip()
            continue

        fields = s.split()
        npipes, nstages, flops = fields[0], fields[1], fields[8]

        if last is not None and npipes != last:
            output.write('\n')
        last = npipes

        output.write(' '.join((npipes, nstages, flops)))
        output.write('\n')

    output.flush()
    return desc


def handle_file(input_filename):
    cmd_file = tempfile.NamedTemporaryFile(
        mode='w+', prefix='pf', suffix='.cmd')
    cmd_file_name = cmd_file.name
    data_file = tempfile.NamedTemporaryFile(
        mode='w+', prefix='pf', suffix='.dat')
    data_file_name = data_file.name
    desc = parse_file(input_filename, data_file)
    if len(desc) > 0:
        cmd_file.write("set title '%s'\n" % (desc,))
    cmd_file.write("set xlabel 'N pipes'\n")
    cmd_file.write("set ylabel 'N stages'\n")
    cmd_file.write("set zlabel 'GFLOPS'\n")
    cmd_file.write("set key off\n")
    cmd_file.write("set view 60, 312\n")
    cmd_file.write("set pm3d\n")
    cmd_file.write("splot '%s' using 1:2:($3*1e-9) with pm3d at b, '%s' using 1:2:($3*1e-9) with pm3d\n" % (
        data_file_name, data_file_name))

    cmd_file.flush()
    data_file.flush()

    os.system("gnuplot " + cmd_file_name + " -")

    # sys.stdout.write(open(cmd_file_name,'r').read())
    # sys.stdout.write(open(data_file_name,'r').read())


def main():
    parser = ArgumentParser()
    parser.add_argument('file', help='Input file')
    args = parser.parse_args()

    handle_file(args.file)


if __name__ == '__main__':
    main()
