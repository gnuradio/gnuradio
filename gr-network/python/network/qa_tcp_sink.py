#!/usr/bin/env python
#
# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import os
import socket
import threading
import time
import unittest

import numpy as np

from gnuradio import blocks, gr, gr_unittest, network

TCPSINKMODE_CLIENT = 1
TCPSINKMODE_SERVER = 2


def get_free_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('127.0.0.1', 0))
    port = s.getsockname()[1]
    s.close()
    return port


class GateBlock(gr.sync_block):
    def __init__(self, data, event):
        gr.sync_block.__init__(self, name="gate_block", in_sig=None, out_sig=[np.uint8])
        self.data = np.array(data, dtype=np.uint8)
        self.event = event
        self.offset = 0

    def work(self, input_items, output_items):
        if not self.event.is_set():
            return 0
        if self.offset >= len(self.data):
            return -1
        out = output_items[0]
        n = min(len(out), len(self.data) - self.offset)
        out[:n] = self.data[self.offset:self.offset + n]
        self.offset += n
        return n


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
        tcp_sink = network.tcp_sink(gr.sizeof_gr_complex, 1, '127.0.0.1', 2000, TCPSINKMODE_CLIENT)
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

    def test_client_mode(self):
        port = get_free_port()
        server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_sock.bind(('127.0.0.1', port))
        server_sock.listen(1)
        server_sock.settimeout(10.0)

        srcdata = tuple(i % 256 for i in range(1000))
        src = blocks.vector_source_b(list(srcdata), False)
        tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', port, TCPSINKMODE_CLIENT)
        self.tb.connect(src, tcp_sink)

        self.tb.start()

        client_sock = None
        try:
            client_sock, address = server_sock.accept()
            client_sock.settimeout(10.0)

            rx_data = bytearray()
            while len(rx_data) < len(srcdata):
                chunk = client_sock.recv(4096)
                if not chunk:
                    break
                rx_data.extend(chunk)

            self.assertEqual(tuple(rx_data), srcdata)
        finally:
            if client_sock:
                client_sock.close()
            server_sock.close()
            self.tb.stop()
            self.tb.wait()

    def test_server_mode(self):
        port = get_free_port()
        srcdata = tuple(i % 256 for i in range(1000))

        connected_event = threading.Event()
        src = GateBlock(srcdata, connected_event)
        tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', port, TCPSINKMODE_SERVER)
        self.tb.connect(src, tcp_sink)

        rx_data = bytearray()
        exception_in_thread = []

        def client_worker():
            try:
                time.sleep(0.05)
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.settimeout(10.0)
                connected = False
                for _ in range(500):
                    try:
                        sock.connect(('127.0.0.1', port))
                        connected = True
                        break
                    except (ConnectionRefusedError, OSError):
                        time.sleep(0.01)
                if not connected:
                    raise RuntimeError("Client worker failed to connect to tcp_sink server")

                time.sleep(0.01)
                connected_event.set()

                while len(rx_data) < len(srcdata):
                    chunk = sock.recv(4096)
                    if not chunk:
                        break
                    rx_data.extend(chunk)
                sock.close()
            except Exception as e:
                exception_in_thread.append(e)

        thread = threading.Thread(target=client_worker)

        try:
            self.tb.start()
            thread.start()

            thread.join(timeout=10.0)
            self.assertFalse(exception_in_thread, f"Exception in client thread: {exception_in_thread}")
            self.assertEqual(tuple(rx_data), srcdata)
        finally:
            self.tb.stop()
            self.tb.wait()

    @unittest.skipIf(os.name == 'nt', "tcp_source is not supported on Windows (issue #7695)")
    def test_e2e_sink_server(self):
        port = get_free_port()
        srcdata = tuple(i % 256 for i in range(1000))

        connected_event = threading.Event()
        tb_sink = gr.top_block()
        src = GateBlock(srcdata, connected_event)
        tcp_sink_block = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', port, TCPSINKMODE_SERVER)
        tb_sink.connect(src, tcp_sink_block)

        tb_source = None
        try:
            tb_sink.start()
            time.sleep(0.05)

            tcp_source_cls = network.tcp_source.tcp_source
            tcp_source_block = tcp_source_cls(gr.sizeof_char, '127.0.0.1', port, server=False)
            time.sleep(0.01)
            connected_event.set()

            tb_source = gr.top_block()
            v_sink = blocks.vector_sink_b()
            tb_source.connect(tcp_source_block, v_sink)
            tb_source.start()

            start_time = time.time()
            while len(v_sink.data()) < len(srcdata) and (time.time() - start_time) < 10.0:
                time.sleep(0.05)

            self.assertEqual(v_sink.data(), srcdata)
        finally:
            tb_sink.stop()
            tb_sink.wait()
            if tb_source:
                tb_source.stop()
                tb_source.wait()

    @unittest.skipIf(os.name == 'nt', "tcp_source is not supported on Windows (issue #7695)")
    def test_e2e_sink_client(self):
        port = get_free_port()
        srcdata = tuple(i % 256 for i in range(1000))

        tb_sink = gr.top_block()
        src = blocks.vector_source_b(list(srcdata), False)
        tcp_sink_block = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', port, TCPSINKMODE_CLIENT)
        tb_sink.connect(src, tcp_sink_block)

        source_holder = []
        exception_holder = []

        tcp_source_cls = network.tcp_source.tcp_source

        def source_worker():
            try:
                src_block = tcp_source_cls(gr.sizeof_char, '127.0.0.1', port, server=True)
                source_holder.append(src_block)
            except Exception as e:
                exception_holder.append(e)

        source_thread = threading.Thread(target=source_worker)
        tb_source = None

        try:
            source_thread.start()
            time.sleep(0.05)

            tb_sink.start()
            source_thread.join(timeout=10.0)

            self.assertFalse(exception_holder, f"Exception in tcp_source server thread: {exception_holder}")
            self.assertTrue(source_holder, "tcp_source server creation timed out")

            tcp_source_block = source_holder[0]
            tb_source = gr.top_block()
            v_sink = blocks.vector_sink_b()
            tb_source.connect(tcp_source_block, v_sink)
            tb_source.start()

            start_time = time.time()
            while len(v_sink.data()) < len(srcdata) and (time.time() - start_time) < 10.0:
                time.sleep(0.05)

            self.assertEqual(v_sink.data(), srcdata)
        finally:
            tb_sink.stop()
            tb_sink.wait()
            if tb_source:
                tb_source.stop()
                tb_source.wait()


if __name__ == '__main__':
    gr_unittest.run(qa_tcp_sink)
