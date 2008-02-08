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
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, eng_notation
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

from gnuradio import gpio

class my_top_block(gr.top_block):

    def __init__(self, options):
        gr.top_block.__init__(self)

	# Create a USRP source with GPIO FPGA build, then configure
        u = usrp.source_s(decim_rate=options.decim,fpga_filename=gpio.fpga_filename)

        if options.force_complex_RXA:
           # This is a dirty hack to force complex mode (receive both I and Q) on basicRX or LFRX
           # This forces the receive board in RXA (side A) to be used 
           # FIXME: This has as a side effect that the gain for Q is not set. So only use with gain 0 (--gain 0)
           options.rx_subdev_spec=(0,0)
           u.set_mux(0x10)
           if not (0==options.gain):
             print "WARNING, you should set the gain to 0 with --gain 0 when using --force-complex-RXA"
             print "The gain for Q will now still be zero while the gain for I is not" 
             #options.gain=0
        else:
          if options.rx_subdev_spec is None:
            options.rx_subdev_spec = usrp.pick_rx_subdevice(u)
          u.set_mux(usrp.determine_rx_mux_value(u, options.rx_subdev_spec))

        subdev = usrp.selected_subdev(u, options.rx_subdev_spec)
        print "Using RX d'board %s" % (subdev.side_and_name(),)
        input_rate = u.adc_freq()/u.decim_rate()
        print "USB sample rate %s" % (eng_notation.num_to_str(input_rate))

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = subdev.gain_range()
            options.gain = float(g[0]+g[1])/2


        #TODO setting gain on basicRX only sets the I channel, use a second subdev to set gain of Q channel
        #see gnuradio-examples/multi-antenna for possible solutions
        subdev.set_gain(options.gain)

        #TODO check if freq has same problem as gain when trying to use complex mode on basicRX
        r = u.tune(0, subdev, options.freq)
        if not r:
            sys.stderr.write('Failed to set frequency\n')
            raise SystemExit, 1

	# Connect pipeline
	src = u
	if options.nsamples is not None:
	    head = gr.head(gr.sizeof_short, int(options.nsamples)*2)
	    self.connect(u, head)
	    src = head

	ana_strip = gpio.and_const_ss(0xFFFE)
	dig_strip = gpio.and_const_ss(0x0001)
        ana_sink = gr.file_sink(gr.sizeof_short, options.ana_filename)
	dig_sink = gr.file_sink(gr.sizeof_short, options.dig_filename)
    
	self.connect(src, ana_strip, ana_sink)
	self.connect(src, dig_strip, dig_sink)

if __name__ == '__main__':
    usage="%prog: [options] analog_filename digital_filename"
    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                      help="select USRP Rx side A or B (default=A)")
    parser.add_option("-d", "--decim", type="int", default=16,
                      help="set fgpa decimation rate to DECIM [default=%default]")
    parser.add_option("-f", "--freq", type="eng_float", default=None,
                      help="set frequency to FREQ", metavar="FREQ")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="set gain in dB (default is midpoint)")
    parser.add_option("-N", "--nsamples", type="eng_float", default=None,
                      help="number of samples to collect [default=+inf]")
    parser.add_option("-F", "--force-complex-RXA", action="store_true", default=False,
                          help="enable basicRX hack to force complex mode on basicRX and LFRX. Only works on side A. Only use with --gain 0")
    (options, args) = parser.parse_args ()
    if len(args) != 2:
        parser.print_help()
        raise SystemExit, 1
    options.ana_filename = args[0]
    options.dig_filename = args[1]

    if options.freq is None:
        parser.print_help()
        sys.stderr.write('You must specify the frequency with -f FREQ\n');
        raise SystemExit, 1

    try:
        tb = my_top_block(options)
	tb.run()
    except KeyboardInterrupt:
        pass
