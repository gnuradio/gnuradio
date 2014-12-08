#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks
import os
import socket
from time import sleep

from threading import Timer
from multiprocessing import Process

class test_tcp_sink(gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb_snd = gr.top_block()
        self.tb_rcv = gr.top_block()

    def tearDown(self):
        self.tb_rcv = None
        self.tb_snd = None

    def _tcp_client(self):
        dst = blocks.vector_sink_s()
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        for t in (0, 0.2):
# wait until server listens
            sleep(t)
            try:
                sock.connect((self.addr, self.port))
            except socket.error as e:
                if e.errno != 111:
                    raise
                continue
            break
        fd = os.dup(sock.fileno())
        self.tb_rcv.connect(blocks.file_descriptor_source(self.itemsize, fd), dst)
        self.tb_rcv.run()
        self.assertEqual(self.data, dst.data())

    def test_001(self):
        self.addr = '127.0.0.1'
        self.port = 65510
        self.itemsize = gr.sizeof_short
        n_data = 16
        self.data = tuple([x for x in range(n_data)])

# tcp_server_sink blocks until client does not connect, start client process first
        p = Process(target=self._tcp_client)
        p.start()

        src = blocks.vector_source_s(self.data, False)
        tcp_snd = blocks.tcp_server_sink(self.itemsize, self.addr, self.port, False)
        self.tb_snd.connect(src, tcp_snd)

        self.tb_snd.run()
        del tcp_snd
        self.tb_snd = None
        p.join()

    def stop_rcv(self):
        self.timeout = True
        self.tb_rcv.stop()
        #print "tb_rcv stopped by Timer"

if __name__ == '__main__':
    gr_unittest.run(test_tcp_sink, "test_tcp_server_sink.xml")

