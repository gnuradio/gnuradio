#!/usr/bin/env python
# 
# Copyright 2013 Institute for Theoretical Information Technology,
#                RWTH Aachen University
# 
# Authors: Johannes Schmitz <schmitz@ti.rwth-aachen.de>
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
import zmqblocks
from gnuradio import gr
from gnuradio import blocks
from gnuradio import analog
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import numpy
import signal
import gui
from PyQt4 import QtGui
import sys


###############################################################################
# GNU Radio top_block
###############################################################################
class top_block(gr.top_block):
    def __init__(self, options):
        gr.top_block.__init__(self)  

        self.options = options

        # create a QT application
        self.qapp = QtGui.QApplication(sys.argv)
        # give Ctrl+C back to system
        signal.signal(signal.SIGINT, signal.SIG_DFL)

        # socket addresses
        rpc_adr_server = "tcp://localhost:6666"
        rpc_adr_reply = "tcp://*:6666"
        probe_adr_gui = "tcp://localhost:5556"
        probe_adr_fg = "tcp://*:5556"
        sink_adr = "tcp://*:5555"
        source_adr = "tcp://localhost:5555"

        # create the main window
        self.ui = gui.gui("Loop",rpc_adr_server,rpc_adr_server,probe_adr_gui)
        self.ui.show()

        # the strange sampling rate gives a nice movement in the plot :P
        self.samp_rate = samp_rate = 48200

        # blocks
        self.gr_sig_source = analog.sig_source_f(samp_rate, analog.GR_SIN_WAVE , 1000, 1, 0)
        self.null_source = blocks.null_source(gr.sizeof_float)
        self.throttle = blocks.throttle(gr.sizeof_float, samp_rate)
        self.zmq_source = zmqblocks.source_pushpull_feedback(gr.sizeof_float,source_adr)
        self.mult_a = blocks.multiply_const_ff(1)
        self.mult_b = blocks.multiply_const_ff(0.5)
        self.add = blocks.add_ff(1)
        #self.zmq_sink = zmqblocks.sink_reqrep_nopoll(gr.sizeof_float, sink_adr)
        #self.zmq_sink = zmqblocks.sink_reqrep(gr.sizeof_float, sink_adr)
        self.zmq_sink = zmqblocks.sink_pushpull(gr.sizeof_float, sink_adr)
        self.zmq_probe = zmqblocks.sink_pushpull(gr.sizeof_float, probe_adr_fg)
        #self.null_sink = blocks.null_sink(gr.sizeof_float)

        # connects
        self.connect(self.gr_sig_source, self.mult_a)
        self.connect(self.zmq_source, self.mult_b, (self.add,1))
#        self.connect(self.null_source, (self.add,1))
        self.connect(self.mult_a, (self.add, 0), self.throttle, self.zmq_sink)
        self.connect(self.throttle, self.zmq_probe)

        # ZeroMQ
        self.rpc_manager = zmqblocks.rpc_manager()
        self.rpc_manager.set_reply_socket(rpc_adr_reply)
        self.rpc_manager.add_interface("start_fg",self.start_fg)
        self.rpc_manager.add_interface("stop_fg",self.stop_fg)
        self.rpc_manager.add_interface("set_waveform",self.set_waveform)
        self.rpc_manager.add_interface("set_k",self.mult_a.set_k)
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
    tb.qapp.exec_()
    tb.stop()
    tb = None
