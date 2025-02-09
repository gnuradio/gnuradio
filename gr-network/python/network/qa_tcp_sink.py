# # #!/usr/bin/env python
# # #
# # # Copyright 2021 Free Software Foundation, Inc.
# # #
# # # This file is part of GNU Radio
# # #
# # # SPDX-License-Identifier: GPL-3.0-or-later

import struct
import gnuradio
import random
from gnuradio import gr, gr_unittest, blocks, network
import socket
import threading
import time
from gnuradio.network.tcp_source import tcp_source
from gnuradio.network import tcp_sink 
import unittest

class qa_tcp_sink(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_client_mode_data_transfer(self):
        """Test tcp_sink in client mode (server=False)."""
        PORT = 3001  # Ensure port > 1024
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind(('localhost', PORT))
        server_socket.listen(1)

        received_data = bytearray()

        def tcp_receive_data(sock, data_buffer):
            """ Accepts a connection and reads data into a buffer. """
            client, addr = sock.accept()
            while True:
                chunk = client.recv(4096)
                if not chunk:
                    break
                data_buffer.extend(chunk)
            client.close()

        recv_thread = threading.Thread(target=tcp_receive_data, args=(server_socket, received_data))
        recv_thread.start()

        test_data = [i % 256 for i in range(100)]
        vector_source = blocks.vector_source_b(test_data, repeat=False)
        tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', PORT, 1)
        vector_sink = blocks.vector_sink_b()

        self.tb.connect(vector_source, tcp_sink)
        self.tb.connect(vector_source, vector_sink)

        self.tb.start()
        self.tb.wait()

        server_socket.close()
        recv_thread.join()

        self.assertEqual(received_data, bytes(test_data))
        self.assertEqual(vector_sink.data(), test_data)


    def test_server_mode_data_transfer(self):
            
            PORT = 3002  # Different port for server mode
            received_data = bytearray()

            def client_thread():

                time.sleep(0.2)  # Ensure the server is listening
                client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                client_socket.connect(('127.0.0.1', PORT))
                test_data = bytes([i % 256 for i in range(100)])
                client_socket.sendall(test_data)
                client_socket.close()

                # Start client thread that will connect after 0.2s
                client = threading.Thread(target=client_thread)
                client.start()

                # Start TCP sink in server mode
                tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', PORT, 1)

                self.tb.start()
                time.sleep(0.5)  # Allow data transfer to complete
                self.tb.stop()
                self.tb.wait()

                client.join()  # Ensure client finished sending

                # Verify received data using a separate receiving socket
                recv_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                recv_socket.settimeout(2)  # Timeout to avoid hanging
                recv_socket.bind(('127.0.0.1', PORT))
                recv_socket.listen(1)

                conn, _ = recv_socket.accept()
                while True:

                    chunk = conn.recv(4096)
                    if not chunk:

                        break
                    received_data.extend(chunk)
                conn.close()
                recv_socket.close()

                # Expected data
                expected_data = [i % 256 for i in range(100)]
                self.assertEqual(received_data, bytes(expected_data))
if __name__ == '__main__':
    gr_unittest.run(qa_tcp_sink)