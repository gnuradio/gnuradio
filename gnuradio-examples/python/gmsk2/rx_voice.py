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
import struct

# from current dir
from receive_path import receive_path

#import os
#print os.getpid()
#raw_input('Attach and press enter')


class audio_tx(gr.hier_block):
    def __init__(self, fg):
        self.packet_src = gr.message_source(33)
        voice_decoder = gsm_full_rate.decode_ps()
        s2f = gr.short_to_float ()
        sink_scale = gr.multiply_const_ff(1.0/32767.)
        audio_sink = audio.sink(8000)
        fg.connect(self.packet_src, voice_decoder, s2f, sink_scale, audio_sink)
        gr.hier_block.__init__(self, fg, self.packet_src, audio_sink)
        
    def msgq(self):
        return self.packet_src.msgq()


class my_graph(gr.flow_graph):

    def __init__(self, rx_subdev_spec, bitrate, decim_rate, spb, rx_callback, log):
        gr.flow_graph.__init__(self)
        self.rxpath = receive_path(self, rx_subdev_spec, bitrate, decim_rate,
                                   spb, rx_callback, log)

        self.audio_tx = audio_tx(self)
        

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

    parser = OptionParser (option_class=eng_option)
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B")
    parser.add_option("-f", "--freq", type="eng_float", default=423.1e6,
                       help="set Rx frequency to FREQ [default=%default]", metavar="FREQ")
    parser.add_option("-r", "--bitrate", type="eng_float", default=100e3,
                      help="specify bitrate.  spb and interp will be derived.")
    parser.add_option("-S", "--spb", type="int", default=None, help="set samples/baud [default=%default]")
    parser.add_option("-d", "--decim", type="intx", default=None,
                      help="set fpga decim rate to DECIM [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=27,
                      help="set rx gain")
    parser.add_option("","--log", action="store_true", default=False,
                      help="enable diagnostic logging")
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    if options.freq < 1e6:
        options.freq *= 1e6

    # build the graph
    fg = my_graph(options.rx_subdev_spec, options.bitrate,
                  options.decim, options.spb, rx_callback, options.log)

    print "bitrate: %sb/sec" % (eng_notation.num_to_str(fg.rxpath.bitrate()),)
    print "spb:     %3d" % (fg.rxpath.spb(),)
    print "decim:   %3d" % (fg.rxpath.decim(),)

    fg.rxpath.set_freq(options.freq)
    fg.rxpath.set_gain(options.gain)
    print "Rx gain_range: ", fg.rxpath.subdev.gain_range(), " using", fg.rxpath.gain

    fg.start()        # start flow graph
    fg.wait()         # wait for it to finish

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
