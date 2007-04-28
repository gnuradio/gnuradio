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
from usrp_sink import usrp_sink_c
from sounder_tx import sounder_tx

n2s = eng_notation.num_to_str

class usrp_sounder_tx(gr.top_block):
    def __init__(self, subdev_spec, freq, cal, verbose, degree, chip_rate, amplitude):

        # Call hierarchical block constructor
        # Top-level blocks have no inputs or outputs
        gr.top_block.__init__(self, "usrp_sounder_tx")
        self._freq = freq
        self._cal = cal
        self._verbose = verbose
        self._degree = degree
        self._length = 2**degree-1
	self._amplitude = amplitude
	
        self._u = usrp_sink_c(0, subdev_spec, chip_rate, self._freq, self._cal, self._verbose)
        self._chip_rate = self._u._if_rate
        self._max_time = float(self._length)/self._chip_rate
	self._pn = sounder_tx(self._degree, self._chip_rate, self._verbose)
        self._gain = gr.multiply_const_ff(amplitude)
        self._f2c = gr.float_to_complex()
	self.connect(self._pn, self._gain, self._f2c, self._u)
        
        if self._verbose:
            print "Chip rate is", n2s(self._chip_rate), "chips/sec"
            print "Resolution is", n2s(1.0/self._chip_rate), "sec"
            print "Using PN code of degree", self._degree, "length", 2**self._degree-1
            print "Maximum measurable impulse response is", n2s(self._max_time), "sec"
            print "Output amplitude is", amplitude

                              
def main():
	parser = OptionParser(option_class=eng_option)

	# Transmit path options
        parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first found)")
        parser.add_option("-f", "--freq", type="eng_float", default=0.0,
                          help="set center frequency (default=%default)")
        parser.add_option("-c", "--cal", type="eng_float", default=0.0,
                          help="set frequency calibration offset (default=%default)")
	parser.add_option("-v", "--verbose", action="store_true", default=False,
			  help="print extra debugging info")
	parser.add_option("-d", "--degree", type="int", default=10,
			  help="set PN code degree (length=2**degree-1, default=%default)")
	parser.add_option("-r", "--chip-rate", type="eng_float", default=8e6,
			  help="set sounder chip rate (default=%default)")
	parser.add_option("-g", "--amplitude", type="eng_float", default=8000.0,
			  help="set output amplitude (default=%default)")
        parser.add_option("", "--real-time", action="store_true", default=False,
                          help="Attempt to enable real-time scheduling")
        (options, args) = parser.parse_args()

	if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        if not options.real_time:
            realtime = False
        else:
            # Attempt to enable realtime scheduling
            r = gr.enable_realtime_scheduling()
            if r == gr.RT_OK:
                realtime = True
            else:
                realtime = False
                print "Note: failed to enable realtime scheduling"

	# Create an instance of a hierarchical block
	top_block = usrp_sounder_tx(options.tx_subdev_spec, options.freq, options.cal,
				    options.verbose, options.degree, options.chip_rate,
				    options.amplitude)
			      
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
