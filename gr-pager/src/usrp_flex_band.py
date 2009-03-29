#!/usr/bin/env python
#
# Copyright 2006,2007,2009 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, usrp, optfir, eng_notation, blks2, pager
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class app_top_block(gr.top_block):
    def __init__(self, options, queue):
	gr.top_block.__init__(self, "usrp_flex_all")
        self.subdev = None

        if options.from_file is not None:
            self.src = gr.file_source(gr.sizeof_gr_complex, options.from_file)
            if options.verbose:
                print "Reading samples from file", options.from_file
        else:
            self.src = usrp.source_c()
            if options.rx_subdev_spec is None:
                options.rx_subdev_spec = usrp.pick_rx_subdevice(self.src)
            self.subdev = usrp.selected_subdev(self.src, options.rx_subdev_spec)
            self.src.set_mux(usrp.determine_rx_mux_value(self.src, options.rx_subdev_spec))
            self.src.set_decim_rate(64)
	    frequency = options.frequency+options.calibration
            result = usrp.tune(self.src, 0, self.subdev, frequency)
            if options.verbose:
                print "Using", self.subdev.name(), " for receiving."
                print "Tuned USRP to", frequency
                
        taps = gr.firdes.low_pass(1.0,
                                  1.0,
                                  1.0/40.0*0.4,
                                  1.0/40.0*0.1,
                                  gr.firdes.WIN_HANN)

        if options.verbose:
            print "Channel filter has", len(taps), "taps"

        bank = blks2.analysis_filterbank(40, taps)
        self.connect(self.src, bank)

        if options.log and options.from_file == None:
            src_sink = gr.file_sink(gr.sizeof_gr_complex, 'usrp.dat')
            self.connect(self.src, src_sink)

        for i in range(40):
	    if i < 20:
		freq = options.frequency+i*25e3
	    else:
		freq = options.frequency-0.5e6+(i-20)*25e3

	    self.connect((bank, i), pager.flex_demod(queue, freq, options.verbose, options.log))
	    if options.log:
		self.connect((bank, i), gr.file_sink(gr.sizeof_gr_complex, 'chan_'+'%3.3f'%(freq/1e6)+'.dat'))


def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-f", "--frequency", type="eng_float", default=929.5e6,
                      help="set receive center frequency to Hz", metavar="Hz")
    parser.add_option("-R", "--rx-subdev-spec", type="subdev",
                      help="select USRP Rx side A or B (default=first daughterboard found)")
    parser.add_option("-c", "--calibration", type="eng_float", default=0.0,
                      help="set frequency offset to Hz", metavar="Hz")
    parser.add_option("-g", "--gain", type="int",
                      help="set RF gain", metavar="dB")
    parser.add_option("-F", "--from-file", default=None,
                      help="Read from file instead of USRP")
    parser.add_option("-l", "--log", action="store_true", default=False,
                      help="log flowgraph to files (LOTS of data)")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="display debug output")
    (options, args) = parser.parse_args()

    if options.verbose:
        print options

    queue = gr.msg_queue()
    tb = app_top_block(options, queue)
    runner = pager.queue_runner(queue)

    try:
        tb.run()
    except KeyboardInterrupt:
        pass

    runner.end()

    
if __name__ == "__main__":
    main()
