#!/usr/bin/env python
#
# Copyright 2005,2006 Free Software Foundation, Inc.
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
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

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

class my_graph(gr.flow_graph):

    def __init__(self, options, mod_kwargs):
        gr.flow_graph.__init__(self)
        self.txpath = transmit_path(self, options, mod_kwargs)


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
                      help="set Tx and Rx frequency to FREQ [default=%default]",
                      metavar="FREQ")
    parser.add_option("-r", "--bitrate", type="eng_float", default=None,
                      help="specify bitrate.  spb and interp will be derived.")
    parser.add_option("-S", "--spb", type="int", default=2,
                      help="set samples/baud [default=%default]")
    parser.add_option("-i", "--interp", type="intx", default=None,
                      help="set fpga interpolation rate to INTERP [default=%default]")
    parser.add_option("-s", "--size", type="eng_float", default=1500,
                      help="set packet size [default=%default]")
    parser.add_option("", "--bt", type="float", default=0.3,
                      help="set bandwidth-time product [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=100.0,
                      help="transmitter gain [default=%default]")
    parser.add_option("-M", "--megabytes", type="eng_float", default=1.0,
                      help="set megabytes to transmit [default=%default]")
    parser.add_option("","--discontinuous", action="store_true", default=False,
                      help="enable discontinous transmission (bursts of 5 packets)")
    fusb_options.add_options(parser)
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    if options.freq < 1e6:
        options.freq *= 1e6

    pkt_size = int(options.size)

    # Add gmsk_mod modulator class to options list
    options.modulation = getattr(blks, "gmsk_mod")

    # Add GMSK modulator's properties
    mod_kwargs = {
        'spb': options.spb,
        'bt' : options.bt,
        'verbose' : False,
        'debug'   : False,
        }
        
    # build the graph
    fg = my_graph(options, mod_kwargs)

    print "bitrate: %sbps" % (eng_notation.num_to_str(fg.txpath.bitrate()),)
    print "spb:     %3d" % (fg.txpath.spb(),)
    print "interp:  %3d" % (fg.txpath.interp(),)

    ok = fg.txpath.set_freq(options.freq)
    if not ok:
        print "Failed to set Tx frequency to %s" % (eng_notation.num_to_str(options.freq),)
        raise SystemExit
    else:
        print "Transmitting on frequency %s" % (eng_notation.num_to_str(options.freq))

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"
    else:
        print "Started realtime scheduling"

    fg.start()                       # start flow graph

    # generate and send packets
    nbytes = int(1e6 * options.megabytes)
    n = 0
    pktno = 0

    while n < nbytes:
        send_pkt(struct.pack('!H', pktno) + (pkt_size - 2) * chr(pktno & 0xff))
        n += pkt_size
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
