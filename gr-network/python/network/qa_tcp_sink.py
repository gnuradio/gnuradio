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


def _recv_exact(conn, nbytes):
    """Receive exactly nbytes from a connected socket."""
    data = b""
    while len(data) < nbytes:
        chunk = conn.recv(nbytes - len(data))
        if not chunk:
            break
        data += chunk
    return data


def _alloc_port():
    """Allocate a TCP port safely."""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("127.0.0.1", 0))
    port = s.getsockname()[1]
    s.listen(1)
    s.close()
    return port


class qa_tcp_sink(gr_unittest.TestCase):

    def test_001_server_false(self):
        """server=False: tcp_sink connects to external socket."""
        payload = bytes(range(50))
        port = _alloc_port()

        srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind(("127.0.0.1", port))
        srv.listen(1)

        received = {}

        def server_thread():
            conn, _ = srv.accept()
            received["data"] = _recv_exact(conn, len(payload))
            conn.close()
            srv.close()

        t = threading.Thread(target=server_thread, daemon=True)
        t.start()

        tb = gr.top_block()
        src = blocks.vector_source_b(list(payload), False)
        sink = network.tcp_sink(gr.sizeof_char, 1, "127.0.0.1", port, False)
        tb.connect(src, sink)
        tb.run()

        t.join(timeout=5.0)
        self.assertEqual(payload, received.get("data", b""))

    def test_002_server_true(self):
        """server=True: tcp_sink listens, client connects."""
        payload = bytes(range(60))
        port = _alloc_port()
        received = {}

        def client_thread():
            deadline = time.time() + 5.0
            while time.time() < deadline:
                try:
                    c = socket.create_connection(("127.0.0.1", port), timeout=2.0)
                    received["data"] = _recv_exact(c, len(payload))
                    c.close()
                    return
                except OSError:
                    time.sleep(0.05)

        t = threading.Thread(target=client_thread, daemon=True)
        t.start()

        tb = gr.top_block()
        src = blocks.vector_source_b(list(payload), False)
        sink = network.tcp_sink(gr.sizeof_char, 1, "127.0.0.1", port, True)
        tb.connect(src, sink)
        tb.run()

        t.join(timeout=5.0)
        self.assertEqual(payload, received.get("data", b""))

    def test_003_end_to_end_sink_server(self):
        """End-to-end: tcp_sink is server, tcp_source connects."""
        payload = bytes(range(40))
        port = _alloc_port()

        tb_sink = gr.top_block()
        src_sink = blocks.vector_source_b(list(payload), False)
        sink = network.tcp_sink(gr.sizeof_char, 1, "127.0.0.1", port, True)
        tb_sink.connect(src_sink, sink)

        tb_src = gr.top_block()
        source = network.tcp_source(gr.sizeof_char, 1, "127.0.0.1", port, False)
        vsink = blocks.vector_sink_b()
        tb_src.connect(source, vsink)

        tb_sink.start()
        tb_src.start()

        tb_sink.wait()
        tb_src.stop()
        tb_src.wait()

        self.assertEqual(list(payload), vsink.data())

    def test_004_end_to_end_source_server(self):
        """End-to-end: tcp_source is server, tcp_sink connects."""
        payload = bytes(range(45))
        port = _alloc_port()

        tb_src = gr.top_block()
        source = network.tcp_source(gr.sizeof_char, 1, "127.0.0.1", port, True)
        vsink = blocks.vector_sink_b()
        tb_src.connect(source, vsink)

        tb_sink = gr.top_block()
        src_sink = blocks.vector_source_b(list(payload), False)
        sink = network.tcp_sink(gr.sizeof_char, 1, "127.0.0.1", port, False)
        tb_sink.connect(src_sink, sink)

        tb_src.start()
        tb_sink.start()

        tb_sink.wait()
        tb_src.stop()
        tb_src.wait()

        self.assertEqual(list(payload), vsink.data())


if __name__ == "__main__":
    gr_unittest.run(qa_tcp_sink)
