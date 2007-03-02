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

from gnuradio import gr, eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from sounder_rx import sounder_rx

def main():
	parser = OptionParser(option_class=eng_option)

	# Receive path options
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first found)")
        parser.add_option("-f", "--freq", type="eng_float", default=0.0,
                          help="set center frequency (default=%default)")
        parser.add_option("-c", "--cal", type="eng_float", default=0.0,
                          help="set frequency calibration offset (default=%default)")
	parser.add_option("-v", "--verbose", action="store_true", default=False,
			  help="print extra debugging info")
	parser.add_option("-d", "--max-delay", type="eng_float", default=10e-6,
			  help="set maximum delay spread (default=%default)")
	parser.add_option("-r", "--chip-rate", type="eng_float", default=8e6,
			  help="set sounder chip rate (default=%default)")
	parser.add_option("-g", "--gain", type="eng_float", default=None,
			  help="set output amplitude (default=%default)")
        (options, args) = parser.parse_args()

	if len(args) != 0:
            parser.print_help()
            sys.exit(1)

	# Create an instance of a hierarchical block
	top_block = sounder_rx(options.rx_subdev_spec, options.freq, options.cal,
	                       options.verbose, options.max_delay, options.chip_rate,
			       options.gain)
			      
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
