#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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

from gnuradio import gr, eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from dect_receiver import dect_receiver
import sys

def main():
	parser = OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first one with a daughterboard)")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("", "--calibration", type="eng_float", default=0.0,
                          help="set frequency calibration offset to FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
	parser.add_option("-v", "--verbose", action="store_true", default=False,
			  help="print extra debugging info")
        parser.add_option("", "--log-baseband", default=None,
                          help="log filtered baseband to file")
        parser.add_option("", "--log-demod", default=None,
                          help="log demodulator output to file")
        (options, args) = parser.parse_args()

	if len(sys.argv) == 1 or len(args) != 0:
            parser.print_help()
            sys.exit(1)
	
	# Create an instance of a hierarchical block
	top_block = dect_receiver(options)
			      
	# Create an instance of a runtime, passing it the top block
	# to process
	runtime = gr.runtime(top_block)

	try:    
            # Run forever
            runtime.run()
	except KeyboardInterrupt:
            # Ctrl-C exits
            pass

if __name__ == '__main__':
    main ()
