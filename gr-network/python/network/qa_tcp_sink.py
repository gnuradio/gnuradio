import socket

from gnuradio import gr, gr_unittest, blocks
from gnuradio.network import tcp_sink


class qa_tcp_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_server_false(self):
        data = list(range(20))

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
        sock.listen(1)

        src = blocks.vector_source_b(data, False)
        sink = tcp_sink(1, "127.0.0.1", port, False)

        self.tb.connect(src, sink)
        self.tb.start()

        conn, _ = sock.accept()
        received = conn.recv(4096)

        self.tb.stop()
        self.tb.wait()

        self.assertEqual(bytes(data), received)
