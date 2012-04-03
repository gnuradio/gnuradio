#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

"""
Reads output from run_synthetic.py and runs gnuplot showing
GFLOPS as f(npipes, nstages)
"""

import re
import sys
import os
import tempfile
from optparse import OptionParser


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
    cmd_file = tempfile.NamedTemporaryFile(mode='w+', prefix='pf', suffix='.cmd')
    cmd_file_name = cmd_file.name
    data_file = tempfile.NamedTemporaryFile(mode='w+', prefix='pf', suffix='.dat')
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
    
    #sys.stdout.write(open(cmd_file_name,'r').read())
    #sys.stdout.write(open(data_file_name,'r').read())


def main():
    usage = "usage: %prog [options] file.dat"
    parser = OptionParser(usage=usage)
    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1

    handle_file(args[0])


if __name__ == '__main__':
    main()
