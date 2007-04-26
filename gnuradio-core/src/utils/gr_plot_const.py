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

class zoom:
    def __init__(self, xdata, reals, imags, sp_iq, sp_const, plot_const, manager):
        self.sp_iq = sp_iq
        self.sp_const = sp_const
        self.xaxis = xdata
        self.reals = reals
        self.imags = imags
        self.plot_const = plot_const
        self.manager = manager

        self.xlim = self.sp_iq.get_xlim()
           
    def __call__(self, event):
        newxlim = self.sp_iq.get_xlim()

        if(newxlim != self.xlim):
            self.xlim = newxlim
            r = self.reals[int(self.xlim[0]) : int(self.xlim[1])]
            i = self.imags[int(self.xlim[0]) : int(self.xlim[1])]

            self.plot_const[0].set_data(r, i)
            self.sp_const.axis([-2, 2, -2, 2])
            self.manager.canvas.draw()
            
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

    iq = gr_read_binary.read_complex_binary(filename)

    if(options.skip is None):
        options.skip = 0
    
    if((options.size is None) or ((options.skip+options.size) > len(iq[0]))):
        options.size = len(iq[0]) - options.skip

    reals = iq[0][options.skip : options.skip + options.size]
    imags = iq[1][options.skip : options.skip + options.size]
    x = range(options.skip, options.skip + options.size)
    
    # PLOT
    f = figure(1, figsize=(16, 12), facecolor='w')
    rcParams['xtick.labelsize'] = 16
    rcParams['ytick.labelsize'] = 16

    # Subplot for real and imaginary parts of signal
    sp1 = f.add_subplot(2,1,1)
    sp1.set_title(("I&Q"), fontsize=26, fontweight="bold")
    sp1.set_xlabel("Time (s)", fontsize=20, fontweight="bold")
    sp1.set_ylabel("Amplitude (V)", fontsize=20, fontweight="bold")
    plot(x, reals, 'bo-', x, imags, 'ro-')

    # Subplot for constellation plot
    sp2 = f.add_subplot(2,1,2)
    sp2.set_title(("Constellation"), fontsize=26, fontweight="bold")
    sp2.set_xlabel("Inphase", fontsize=20, fontweight="bold")
    sp2.set_ylabel("Qaudrature", fontsize=20, fontweight="bold")
    p2 = plot(reals, imags, 'bo')
    sp2.axis([-2, 2, -2, 2])
    
    manager = get_current_fig_manager()
    zm = zoom(x, reals, imags, sp1, sp2, p2, manager)
    connect('draw_event', zm)
    
    show()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    


