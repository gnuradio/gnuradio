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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, gru, blks
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import random
import struct

# from current dir
from bpsk import bpsk_demod
from dbpsk import dbpsk_demod
from dqpsk import dqpsk_demod
from receive_path import receive_path
import fusb_options

if 1:
    import os
    print os.getpid()
    raw_input('Attach and press enter')


class my_graph(gr.flow_graph):

    def __init__(self, demod_class, rx_subdev_spec,
                 bitrate, decim_rate, spb,
                 rx_callback, options, demod_kwargs):
        gr.flow_graph.__init__(self)
        self.rxpath = receive_path(self, demod_class, rx_subdev_spec,
                                   bitrate, decim_rate, spb,
                                   rx_callback, options, demod_kwargs)

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
        (pktno,) = struct.unpack('!H', payload[0:2])
        n_rcvd += 1
        if ok:
            n_right += 1

        print "ok = %r  pktno = %4d  n_rcvd = %4d  n_right = %4d" % (
            ok, pktno, n_rcvd, n_right)

    parser = OptionParser (option_class=eng_option)
    parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                      help="select USRP Rx side A or B")
    parser.add_option("-f", "--freq", type="eng_float", default=423.1e6,
                      help="set Rx frequency to FREQ [default=%default]",
                      metavar="FREQ")
    parser.add_option("-r", "--bitrate", type="eng_float", default=None,
                      help="specify bitrate.  spb and interp will be derived.")
    parser.add_option("-S", "--spb", type="int", default=None,
                      help="set samples/baud [default=%default]")
    parser.add_option("-d", "--decim", type="intx", default=None,
                      help="set fpga decim rate to DECIM [default=%default]")
    parser.add_option("-m", "--modulation", type="string", default='dbpsk',
                      help="modulation type (bpsk, dbpsk, dqpsk) [default=%default]")
    parser.add_option("", "--excess-bw", type="float", default=0.3,
                      help="set RRC excess bandwith factor [default=%default]")
    parser.add_option("-g", "--gain", type="eng_float", default=27,
                      help="set rx gain")
    parser.add_option("","--log", action="store_true", default=False,
                      help="enable diagnostic logging")
    fusb_options.add_options(parser)
    (options, args) = parser.parse_args ()

    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    if options.freq < 1e6:
        options.freq *= 1e6

    demod_kwargs = {
            'excess_bw' : options.excess_bw,
            }

    #FIXME: Needs to be worked in to overall structure; this will be fixed
    #       once static class definitions for modulations are defined
    if(options.modulation=='bpsk'):
        modulation=bpsk_demod
    elif(options.modulation=='dbpsk'):
        modulation=dbpsk_demod
    else:
        modulation=dqpsk_demod

    # build the graph
    fg = my_graph(modulation,
                  options.rx_subdev_spec, options.bitrate,
                  options.decim, options.spb,
                  rx_callback, options, demod_kwargs)

    print "bitrate: %sb/sec" % (eng_notation.num_to_str(fg.rxpath.bitrate()),)
    print "spb:     %3d" % (fg.rxpath.spb(),)
    print "decim:   %3d" % (fg.rxpath.decim(),)

    ok = fg.rxpath.set_freq(options.freq)
    if not ok:
        print "Failed to set Rx frequency to %s" % (eng_notation.num_to_str(options.freq),)
        raise SystemExit
    
    fg.rxpath.set_gain(options.gain)
    print "Rx gain_range: ", fg.rxpath.subdev.gain_range(), " using", fg.rxpath.gain

    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: Failed to enable realtime scheduling."

    fg.start()        # start flow graph
    fg.wait()         # wait for it to finish

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
