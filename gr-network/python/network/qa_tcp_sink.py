#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, network
import socket
import threading
import time


class qa_tcp_sink (gr_unittest.TestCase):
    def tcp_receive(self, serversocket):
        for _ in range(2):
            clientsocket, address = serversocket.accept()
            while True:
                data = clientsocket.recv(4096)
                if not data:
                    break
            clientsocket.close()

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_restart(self):
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversocket.settimeout(30.0)
        serversocket.bind(('localhost', 2000))
        serversocket.listen()

        thread = threading.Thread(target=self.tcp_receive, args=(serversocket,))
        thread.start()

        null_source = blocks.null_source(gr.sizeof_gr_complex)
        throttle = blocks.throttle(gr.sizeof_gr_complex, 320000, True)
        tcp_sink = network.tcp_sink(gr.sizeof_gr_complex, 1, '127.0.0.1', 2000, 1)
        self.tb.connect(null_source, throttle, tcp_sink)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()
        time.sleep(0.1)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()

        thread.join()
        serversocket.close()


if __name__ == '__main__':
    gr_unittest.run(qa_tcp_sink)
