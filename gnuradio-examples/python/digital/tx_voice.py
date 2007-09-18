#!/usr/bin/env python
#
# Copyright 2005,2006,2007 Free Software Foundation, Inc.
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
import time
import struct
import sys

# from current dir
from transmit_path import transmit_path
import fusb_options

#import os
#print os.getpid()
#raw_input('Attach and press enter')


class audio_rx(gr.hier_block2):
    def __init__(self, audio_input_dev):
	gr.hier_block2.__init__(self, "audio_rx",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(0, 0, 0)) # Output signature
        sample_rate = 8000
        src = audio.source(sample_rate, audio_input_dev)
        src_scale = gr.multiply_const_ff(32767)
        f2s = gr.float_to_short()
        voice_coder = gsm_full_rate.encode_sp()
        self.packets_from_encoder = gr.msg_queue()
        packet_sink = gr.message_sink(33, self.packets_from_encoder, False)
        self.connect(src, src_scale, f2s, voice_coder, packet_sink)

    def get_encoded_voice_packet(self):
        return self.packets_from_encoder.delete_head()
        

class my_top_block(gr.top_block):

    def __init__(self, modulator_class, options):
        gr.top_block.__init__(self)
        self.txpath = transmit_path(modulator_class, options)
        self.audio_rx = audio_rx(options.audio_input)
	self.connect(self.txpath)
	self.connect(self.audio_rx)


# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():

    def send_pkt(payload='', eof=False):
        return tb.txpath.send_pkt(payload, eof)

    def rx_callback(ok, payload):
        print "ok = %r, payload = '%s'" % (ok, payload)

    mods = modulation_utils.type_1_mods()

    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    expert_grp = parser.add_option_group("Expert")

    parser.add_option("-m", "--modulation", type="choice", choices=mods.keys(),
                      default='gmsk',
                      help="Select modulation from: %s [default=%%default]"
                            % (', '.join(mods.keys()),))
    parser.add_option("-M", "--megabytes", type="eng_float", default=0,
                      help="set megabytes to transmit [default=inf]")
    parser.add_option("-I", "--audio-input", type="string", default="",
                      help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")

    transmit_path.add_options(parser, expert_grp)

    for mod in mods.values():
        mod.add_options(expert_grp)

    fusb_options.add_options(expert_grp)

    parser.set_defaults(bitrate=50e3)  # override default bitrate default
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    if options.tx_freq is None:
        sys.stderr.write("You must specify -f FREQ or --freq FREQ\n")
        parser.print_help(sys.stderr)
        sys.exit(1)


    # build the graph
    tb = my_top_block(mods[options.modulation], options)

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"


    tb.start()                       # start flow graph

    # generate and send packets
    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0

    while nbytes == 0 or n < nbytes:
        packet = tb.audio_rx.get_encoded_voice_packet()
        s = packet.to_string()
        send_pkt(s)
        n += len(s)
        sys.stderr.write('.')
        pktno += 1
        
    send_pkt(eof=True)
    tb.wait()                       # wait for it to finish
    tb.txpath.set_auto_tr(False)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
	pass