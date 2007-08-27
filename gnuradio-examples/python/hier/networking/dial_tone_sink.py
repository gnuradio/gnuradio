#!/usr/bin/env python
#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class dial_tone_sink(gr.top_block):
    def __init__(self, src, port, pkt_size, sample_rate):
        gr.top_block.__init__(self, "dial_tone_sink")
        udp = gr.udp_source(gr.sizeof_float, src, port, pkt_size)
        sink = audio.sink(sample_rate)
        self.connect(udp, sink)
        
if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option)
    parser.add_option("", "--src-name", type="string", default="localhost",
                      help="local host name (domain name or IP address)")
    parser.add_option("", "--src-port", type="int", default=65500,
                      help="port value to listen to for connection")
    parser.add_option("", "--packet-size", type="int", default=1472,
                      help="packet size.")
    parser.add_option("-r", "--sample-rate", type="int", default=8000,
                      help="audio signal sample rate [default=%default]")
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        raise SystemExit, 1

    # Create an instance of a hierarchical block
    top_block = dial_tone_sink(options.src_name, options.src_port,
                               options.packet_size, options.sample_rate)
    
    try:    
        # Run forever
        top_block.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass
    
