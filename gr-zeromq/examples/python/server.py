#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

###############################################################################
# Imports
###############################################################################
from gnuradio import zeromq
from gnuradio import gr
from gnuradio import blocks
from gnuradio import analog
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import numpy
import sys
from threading import Thread
import time


###############################################################################
# GNU Radio top_block
###############################################################################
class top_block(gr.top_block):
    def __init__(self, options):
        gr.top_block.__init__(self)

        self.options = options

        # socket addresses
        rpc_adr = "tcp://*:6666"
        probe_adr = "tcp://*:5556"
        sink_adr = "tcp://*:5555"

        # the strange sampling rate gives a nice movement in the plot :P
        self.samp_rate = samp_rate = 48200

        # blocks
        self.gr_sig_source = analog.sig_source_f(samp_rate, analog.GR_SIN_WAVE , 1000, 1, 0)
        self.throttle = blocks.throttle(gr.sizeof_float, samp_rate)
        self.mult = blocks.multiply_const_ff(1)
        #self.zmq_sink = zeromq.rep_sink(gr.sizeof_float, 1, sink_adr)
        self.zmq_sink = zeromq.pub_sink(gr.sizeof_float, 1, sink_adr)
        #self.zmq_sink = zeromq.push_sink(gr.sizeof_float, 1, sink_adr)
        #self.zmq_probe = zeromq.push_sink(gr.sizeof_float, 1, probe_adr)
        self.zmq_probe = zeromq.pub_sink(gr.sizeof_float, 1, probe_adr)
        #self.null_sink = blocks.null_sink(gr.sizeof_float)

        # connects
        self.connect(self.gr_sig_source, self.mult, self.throttle, self.zmq_sink)
        self.connect(self.throttle, self.zmq_probe)

        # ZeroMQ
        self.rpc_manager = zeromq.rpc_manager()
        self.rpc_manager.set_reply_socket(rpc_adr)
        self.rpc_manager.add_interface("start_fg",self.start_fg)
        self.rpc_manager.add_interface("stop_fg",self.stop_fg)
        self.rpc_manager.add_interface("set_waveform",self.set_waveform)
        self.rpc_manager.add_interface("set_k",self.mult.set_k)
        self.rpc_manager.add_interface("get_sample_rate",self.throttle.sample_rate)
        self.rpc_manager.start_watcher()

    def start_fg(self):
        print "Start Flowgraph"
        try:
            self.start()
        except RuntimeError:
            print "Can't start, flowgraph already running!"

    def stop_fg(self):
        print "Stop Flowgraph"
        self.stop()
        self.wait()

    def set_waveform(self, waveform_str):
        waveform = {'Constant' : analog.GR_CONST_WAVE,
                    'Sine' : analog.GR_SIN_WAVE,
                    'Cosine' : analog.GR_COS_WAVE,
                    'Square' : analog.GR_SQR_WAVE,
                    'Triangle' : analog.GR_TRI_WAVE,
                    'Saw Tooth' : analog.GR_SAW_WAVE}[waveform_str]
        self.gr_sig_source.set_waveform(waveform)

###############################################################################
# Options Parser
###############################################################################
def parse_options():
    """ Options parser. """
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    return options

###############################################################################
# Main
###############################################################################
if __name__ == "__main__":
    options = parse_options()
    tb = top_block(options)
    try:
        # keep the program running when flowgraph is stopped
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    print "Shutting down flowgraph."
    tb.rpc_manager.stop_watcher()
    tb.stop()
    tb.wait()
    tb = None
