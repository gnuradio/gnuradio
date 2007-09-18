#!/usr/bin/env python
#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, modulation_utils
from gnuradio import usrp
from gnuradio import audio
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

from gnuradio.vocoder import gsm_full_rate

import random
import struct
import sys

# from current dir
from receive_path import receive_path
import fusb_options

#import os
#print os.getpid()
#raw_input('Attach and press enter')


class audio_tx(gr.hier_block2):
    def __init__(self, audio_output_dev):
	gr.hier_block2.__init__(self, "audio_tx",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature
				
        self.packet_src = gr.message_source(33)
        voice_decoder = gsm_full_rate.decode_ps()
        s2f = gr.short_to_float ()
        sink_scale = gr.multiply_const_ff(1.0/32767.)
        audio_sink = audio.sink(8000, audio_output_dev)
        self.connect(self.packet_src, voice_decoder, s2f, sink_scale, audio_sink)
        
    def msgq(self):
        return self.packet_src.msgq()


class my_top_block(gr.top_block):
    def __init__(self, demod_class, rx_callback, options):
        gr.top_block.__init__(self)
        self.rxpath = receive_path(demod_class, rx_callback, options)
        self.audio_tx = audio_tx(options.audio_output)
	self.connect(self.rxpath)
	self.connect(self.audio_tx)        

# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

global n_rcvd, n_right

def main():
    global n_rcvd, n_right

    n_rcvd = 0
    n_right = 0
    
    def rx_callback(ok, payload):
        global n_rcvd, n_right
        n_rcvd += 1
        if ok:
            n_right += 1

        fg.audio_tx.msgq().insert_tail(gr.message_from_string(payload))
        
        print "ok = %r  n_rcvd = %4d  n_right = %4d" % (
            ok, n_rcvd, n_right)

    demods = modulation_utils.type_1_demods()

    # Create Options Parser:
    parser = OptionParser (option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")

    parser.add_option("-m", "--modulation", type="choice", choices=demods.keys(), 
                      default='gmsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(demods.keys()),))
    parser.add_option("-O", "--audio-output", type="string", default="",
                      help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")

    receive_path.add_options(parser, expert_grp)

    for mod in demods.values():
        mod.add_options(expert_grp)

    fusb_options.add_options(expert_grp)

    parser.set_defaults(bitrate=50e3)  # override default bitrate default
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help(sys.stderr)
        sys.exit(1)

    if options.rx_freq is None:
        sys.stderr.write("You must specify -f FREQ or --freq FREQ\n")
        parser.print_help(sys.stderr)
        sys.exit(1)


    # build the graph
    tb = my_top_block(demods[options.modulation], rx_callback, options)

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: Failed to enable realtime scheduling."

    tb.run()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
