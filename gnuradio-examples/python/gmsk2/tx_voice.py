#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, blks
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

#import os
#print os.getpid()
#raw_input('Attach and press enter')


class my_graph(gr.flow_graph):

    def __init__(self, tx_subdev_spec, bitrate, interp_rate, spb, bt):
        gr.flow_graph.__init__(self)
        self.txpath = transmit_path(self, tx_subdev_spec, bitrate, interp_rate, spb, bt)

        audio_input = ""
        sample_rate = 8000
        src = audio.source(sample_rate, audio_input)
        src_scale = gr.multiply_const_ff(32767)
        f2s = gr.float_to_short()
        voice_coder = gsm_full_rate.encode_sp()
        self.packets_from_encoder = gr.msg_queue()
        packet_sink = gr.message_sink(33, self.packets_from_encoder, False)
        self.connect(src, src_scale, f2s, voice_coder, packet_sink)



# /////////////////////////////////////////////////////////////////////////////
#                                   main
# /////////////////////////////////////////////////////////////////////////////

def main():

    def send_pkt(payload='', eof=False):
        return fg.txpath.send_pkt(payload, eof)

    def rx_callback(ok, payload):
        print "ok = %r, payload = '%s'" % (ok, payload)

    parser = OptionParser (option_class=eng_option)
    parser.add_option("-T", "--tx-subdev-spec", type="subdev", default=None,
                      help="select USRP Tx side A or B")
    parser.add_option("-f", "--freq", type="eng_float", default=423.1e6,
                       help="set Tx and Rx frequency to FREQ [default=%default]", metavar="FREQ")
    parser.add_option("-r", "--bitrate", type="eng_float", default=100e3,
                      help="specify bitrate.  spb and interp will be derived.")
    parser.add_option("-S", "--spb", type="int", default=None, help="set samples/baud [default=%default]")
    parser.add_option("-i", "--interp", type="intx", default=None,
                      help="set fpga interpolation rate to INTERP [default=%default]")
    parser.add_option("-s", "--size", type="eng_float", default=1500,
                      help="set packet size [default=%default]")
    parser.add_option("", "--bt", type="float", default=0.3, help="set bandwidth-time product [default=%default]")
    parser.add_option("-M", "--megabytes", type="eng_float", default=1.0,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinous transmission (bursts of 5 packets)")
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    if options.freq < 1e6:
        options.freq *= 1e6

    pkt_size = int(options.size)

    # build the graph
    fg = my_graph(options.tx_subdev_spec, options.bitrate, options.interp,
                  options.spb, options.bt)

    print "bitrate: %sb/sec" % (eng_notation.num_to_str(fg.txpath.bitrate()),)
    print "spb:     %3d" % (fg.txpath.spb(),)
    print "interp:  %3d" % (fg.txpath.interp(),)

    fg.txpath.set_freq(options.freq)

    fg.start()                       # start flow graph

    # generate and send packets
    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0

    while n < nbytes:
        packet = fg.packets_from_encoder.delete_head()
        s = packet.to_string()
        send_pkt(s)
        n += len(s)
        sys.stderr.write('.')
        if options.discontinuous and pktno % 5 == 4:
            time.sleep(1)
        pktno += 1
        
    send_pkt(eof=True)
    fg.wait()                       # wait for it to finish
    fg.txpath.set_auto_tr(False)


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
