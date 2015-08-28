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
#import zeromq
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
        rpc_adr = "tcp://*:6667"
        probe_adr = "tcp://*:5557"
        source_adr = "tcp://"+self.options.servername+":5555"

        # blocks
        #self.zmq_source = zeromq.req_source(gr.sizeof_float, 1, source_adr)
        #self.zmq_source = zeromq.pull_source(gr.sizeof_float, 1, source_adr)
        self.zmq_source = zeromq.sub_source(gr.sizeof_float, 1, source_adr)
        #self.zmq_probe = zeromq.push_sink(gr.sizeof_float, 1, probe_adr)
        self.zmq_probe = zeromq.pub_sink(gr.sizeof_float, 1, probe_adr)

        # connects
        self.connect(self.zmq_source, self.zmq_probe)

        # ZeroMQ
        self.rpc_manager = zeromq.rpc_manager()
        self.rpc_manager.set_reply_socket(rpc_adr)
        self.rpc_manager.add_interface("start_fg",self.start_fg)
        self.rpc_manager.add_interface("stop_fg",self.stop_fg)
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

###############################################################################
# Options Parser
###############################################################################
def parse_options():
    """ Options parser. """
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("-s", "--servername", type="string", default="localhost",
                      help="Server hostname")
    (options, args) = parser.parse_args()
    return options

###############################################################################
# Waiter Thread
###############################################################################
class waiter(Thread):
    """ To keep the program alive when flowgraph is stopped. """
    def run(self):
        while keep_running:
            time.sleep(1)

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
