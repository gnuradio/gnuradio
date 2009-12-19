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

import os
import os.path
import sys
from gnuradio.eng_option import eng_option
from optparse import OptionParser

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-d", "--decim", type="int", default=16,
                      help="set fgpa decimation rate to DECIM [default=%default]")
    parser.add_option("-f", "--freq", type="eng_float", default=0.0,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-g", "--gain", type="string", default=None,
                      help="set gain to GAIN [default=%default]")
    parser.add_option("-W", "--waterfall", action="store_true", default=False,
                      help="Enable waterfall display")
    parser.add_option("-S", "--oscilloscope", action="store_true", default=False,
                      help="Enable oscilloscope display")
    parser.add_option("-F", "--samples-per-frame", type="int", default=250,
                      help="[default=%default]")
    parser.add_option("-e", "--eth", default="eth0",
                      help="specify ethernet interface [default=%default]")

    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)


    path = os.path.dirname(sys.argv[0])
    if path == '':
        path = '.'
    
    
    display_type = ''
    if options.waterfall:
        display_type = '-W'
    if options.oscilloscope:
        display_type = '-S'

    gain_clause = ''
    if options.gain:
        gain_clause = '-g ' + options.gain

    # FIXME: restore -F
    cmd = "%s/rx_streaming_samples -s -e %s -f %g -d %d %s -o /proc/self/fd/1 | %s/stdin_int32_fft.py %s -f %g -d %d" % (
        path, options.eth, options.freq, options.decim, gain_clause,
        path, display_type, options.freq, options.decim)

    print cmd
    os.system(cmd)
    

if __name__ == '__main__':
    main()
