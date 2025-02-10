import gnuradio
from gnuradio import gr, gr_unittest, blocks, network
import socket
import threading
import time


class qa_tcp_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.port = 2003  # Port > 1024 for test
        self.received_data = bytearray()

    def tcp_client(self, test_data):
        """Client will connect to tcp_sink and send the test data."""
        time.sleep(0.1)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect(('localhost', self.port))
            client_socket.sendall(test_data)
            client_socket.close()

    def tearDown(self):
        self.tb = None
        self.received_data = bytearray()  # Reset received data for the next test

    def tcp_server(self):
        """TCP Server to receive and store data sent by tcp_sink."""
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server_socket.bind(('localhost', self.port))
            server_socket.listen(1)
            client_socket, _ = server_socket.accept()
            with client_socket:
                while True:
                    data = client_socket.recv(4096)
                    if not data:
                        break
                    self.received_data.extend(data)

    def test_server_mode(self):
        """Test tcp_sink in server mode (sending known data)."""
        test_data = list(b'hello world')  # Send known data to the server

        # Create the GNU Radio flowgraph
        vector_source = blocks.vector_source_b(test_data, False)
        tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', self.port, 1)
        self.tb.connect(vector_source, tcp_sink)

        # Start the server thread
        server_thread = threading.Thread(target=self.tcp_server, daemon=True)
        server_thread.start()
        time.sleep(0.1)  # Ensure server is ready

        # Run the flowgraph
        self.tb.start()
        time.sleep(0.5)  # Allow time for transfer
        self.tb.stop()
        self.tb.wait()

        # Validate received data
        self.assertEqual(self.received_data, bytearray(test_data))

    def test_tcp_sink_client_mode(self):
        """Test tcp_sink in client mode (server=False) by sending known data."""
        # Start TCP Server in background thread
        server_thread = threading.Thread(target=self.tcp_server, daemon=True)
        server_thread.start()
        time.sleep(0.1)  # Ensure server is ready before starting flowgraph

        # Known data to send
        test_data = list(b'hello world')

        # Create the GNU Radio flowgraph
        vector_source = blocks.vector_source_b(test_data, False)
        tcp_sink = network.tcp_sink(gr.sizeof_char, 1, '127.0.0.1', self.port, 1)
        self.tb.connect(vector_source, tcp_sink)

        # Start client communication
        client_thread = threading.Thread(target=self.tcp_client, args=(bytes(test_data),))
        client_thread.start()

        # Run the flowgraph
        self.tb.start()
        time.sleep(0.5)  # Allow time for data transfer
        self.tb.stop()
        self.tb.wait()

        # Ensure the server receives the same data sent by client
        client_thread.join()
        self.assertEqual(self.received_data, bytearray(test_data))


if __name__ == '__main__':
    gr_unittest.run(qa_tcp_sink)
