#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

import pylab, math
from pylab import *
import struct, sys
from optparse import OptionParser

import gr_read_binary

def main():
    usage="%prog: [options] output_filename"
    parser = OptionParser(conflict_handler="resolve", usage=usage)
    parser.add_option("-s", "--size", type="int", default=None,
                      help="Specify the number of points to plot [default=%default]")
    parser.add_option("", "--skip", type="int", default=None,
                      help="Specify the number of points to skip [default=%default]")

    (options, args) = parser.parse_args ()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1
    filename = args[0]

    fl = gr_read_binary.read_float_binary(filename)

    if(options.skip is None):
        options.skip = 0
    
    if((options.size is None) or ((options.skip+options.size) > len(iq[0]))):
        options.size = len(fl) - options.skip

    x = range(options.skip, options.skip + options.size)
    
    # PLOT REAL AND IMAGINARY PARTS
    
    f = figure(1, figsize=(16, 12), facecolor='w')
    rcParams['xtick.labelsize'] = 16
    rcParams['ytick.labelsize'] = 16

    sp1 = f.add_subplot(1,1,1)
    sp1.set_title(("GNU Radio Float"), fontsize=26, fontweight="bold")
    sp1.set_xlabel("Time (s)", fontsize=20, fontweight="bold")
    sp1.set_ylabel("Amplitude (V)", fontsize=20, fontweight="bold")
    plot(x, fl, 'bo-')

    show()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    


