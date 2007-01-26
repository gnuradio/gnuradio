#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class dial_tone_source(gr.hier_block2):
    def __init__(self, local_ipaddress, remote_ipaddress, port, mtu, sample_rate):
        gr.hier_block2.__init__(self, 
                                "dial_tone_source",	# Block type 
                                gr.io_signature(0,0,0), # Input signature
                                gr.io_signature(0,0,0)) # Output signature

        amplitude = 0.3
        self.define_component("src0", gr.sig_source_f (sample_rate, gr.GR_SIN_WAVE,
                                                       350, amplitude))
        self.define_component("src1", gr.sig_source_f (sample_rate, gr.GR_SIN_WAVE,
                                                       440, amplitude))
        self.define_component("add", gr.add_ff())

	self.define_component("thr", gr.throttle(gr.sizeof_float, sample_rate))
	self.define_component("dst",  gr.udp_sink(gr.sizeof_float,
                                                  local_ipaddress, 0,
                                                  remote_ipaddress, port,
                                                  mtu))
        
        self.connect("src0", 0, "add", 0)	
        self.connect("src1", 0, "add", 1)
	self.connect("add", 0, "thr", 0)
	self.connect("thr", 0, "dst", 0)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option)
    parser.add_option("", "--local-ipaddr", type="string", default="127.0.0.1",
                      help="local IP address")
    parser.add_option("", "--remote-ipaddr", type="string", default="127.0.0.1",
                      help="Remote IP address")
    parser.add_option("", "--remote-port", type="int", default=65500,
                      help="port value to connect to")
    parser.add_option("", "--mtu", type="int", default=540,
                     help="packet size.")
    parser.add_option("-r", "--sample-rate", type="int", default=8000,
    		     help="audio signal sample rate [default=%default]")
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        raise SystemExit, 1

    # Create an instance of a hierarchical block
    top_block = dial_tone_source(options.local_ipaddr, options.remote_ipaddr,
                                 options.remote_port, options.mtu, options.sample_rate)
    
    # Create an instance of a runtime, passing it the top block
    runtime = gr.runtime(top_block)
    
    try:    
        # Run forever
        runtime.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass
    
