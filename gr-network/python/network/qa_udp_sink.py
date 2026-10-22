#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, network
import time
import socket
import threading


class qa_udp_sink(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        self.thread_stopped = False
        self.receive_ready = False

    def tearDown(self):
        self.tb = None

    def run_socket_receiver_thread(self, skt):
        self.packets = []
        while not self.thread_stopped:
            try:
                pkt, _ = skt.recvfrom(9000)
                # Keep receiving packets until timeout
                while pkt:
                    self.packets.append(pkt)
                    pkt, _ = skt.recvfrom(9000)

            except socket.timeout:
                # Set flag after first timeout to avoid race condition
                self.receive_ready = True
        skt.close()

    def make_socket(self, addr, port):
        skt = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        skt.bind((addr, port))
        skt.settimeout(0.1)
        return skt

    def test_restart(self):
        null_source = blocks.null_source(gr.sizeof_gr_complex)
        throttle = blocks.throttle(gr.sizeof_gr_complex, 320000)
        udp_sink = network.udp_sink(gr.sizeof_gr_complex, 1, '127.0.0.1', 2000,
                                    0, 1472, False)
        self.tb.connect(null_source, throttle, udp_sink)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()
        time.sleep(0.1)
        self.tb.start()
        time.sleep(0.1)
        self.tb.stop()

    def test_ten_packets(self):
        skt = self.make_socket("127.0.0.1", 12345)
        rxthread = threading.Thread(target=self.run_socket_receiver_thread, args=(skt,))
        rxthread.start()
        NUM_PKTS = 10
        PAYLOAD_SZ = 1472
        input_src = [1] * PAYLOAD_SZ * NUM_PKTS
        src = blocks.vector_source_b(input_src)
        snk = network.udp_sink(
            gr.sizeof_char, 1, "127.0.0.1", 12345, 0, PAYLOAD_SZ, True
        )
        self.tb.connect(src, snk)
        while not self.receive_ready:
            time.sleep(0.001)
        self.tb.run()
        self.thread_stopped = True
        rxthread.join()
        bytes_received = [i for pkt in self.packets for i in pkt]
        self.assertListEqual(input_src, bytes_received)

    def test_veclen(self):
        skt = self.make_socket("127.0.0.1", 12346)
        rxthread = threading.Thread(target=self.run_socket_receiver_thread, args=(skt,))
        rxthread.start()
        VECLEN = 8
        NUMPKTS = 100
        PAYLOAD_SZ = 1472
        input_src = [2] * NUMPKTS * PAYLOAD_SZ
        src = blocks.vector_source_b(input_src, vlen=VECLEN)
        snk = network.udp_sink(
            gr.sizeof_char, VECLEN, "127.0.0.1", 12346, 0, PAYLOAD_SZ, True
        )

        self.tb.connect(src, snk)
        while not self.receive_ready:
            time.sleep(0.001)
        self.tb.run()
        self.thread_stopped = True
        rxthread.join()
        self.assertEqual(NUMPKTS, len(self.packets))
        bytes_received = [i for pkt in self.packets for i in pkt]
        self.assertListEqual(input_src, bytes_received)


if __name__ == '__main__':
    gr_unittest.run(qa_udp_sink)
