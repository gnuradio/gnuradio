#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
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
import numpy
import os
import socket
import time

from threading import Timer, Thread


def recv_data(sock, result):
    while True:
        data = sock.recv(4*1000)
        if len(data) == 0:
            break
        real_data = numpy.frombuffer(data, dtype=numpy.float32)
        result.extend(list(real_data))


class test_udp_sink_source(gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb_snd = gr.top_block()
        self.tb_rcv = gr.top_block()

    def tearDown(self):
        self.tb_rcv = None
        self.tb_snd = None

    def test_001(self):
        # Tests calling disconnect/reconnect.

        port = 65510

        n_data = 16
        src_data = [x for x in range(n_data)]
        expected_result = tuple(src_data)
        src = blocks.vector_source_s(src_data, False)
        udp_snd = blocks.udp_sink(gr.sizeof_short, 'localhost', port)
        self.tb_snd.connect(src, udp_snd)

        self.tb_snd.run()
        udp_snd.disconnect()


        udp_snd.connect('localhost', port+1)
        src.rewind()
        self.tb_snd.run()


    def test_sink_001(self):
        port = 65520

        n_data = 100
        src_data = [float(x) for x in range(n_data)]
        expected_result = tuple(src_data)

        recvsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        recvsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        recvsock.bind(('127.0.0.1', port))

        result = []
        t = Thread(target=recv_data, args=(recvsock, result))
        t.start()

        src = blocks.vector_source_f(src_data, False)
        udp_snd = blocks.udp_sink(gr.sizeof_float, '127.0.0.1', port)
        self.tb_snd.connect(src, udp_snd)


        self.tb_snd.run()
        udp_snd.disconnect()
        t.join()
        recvsock.close()

        self.assertEqual(expected_result, tuple(result))

    def test_source_001(self):
        port = 65520

        n_data = 100
        src_data = [float(x) for x in range(n_data)]
        expected_result = tuple(src_data)
        send_data = numpy.array(src_data, dtype=numpy.float32)
        send_data = send_data.tobytes()

        udp_rcv = blocks.udp_source(gr.sizeof_float, '127.0.0.1', port)
        dst = blocks.vector_sink_f()
        self.tb_rcv.connect(udp_rcv, dst)
        self.tb_rcv.start()
        time.sleep(1.0)
        sendsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sendsock.sendto(send_data, ('127.0.0.1', port))
        time.sleep(1.0)
        sendsock.sendto(b'', ('127.0.0.1', port))
        sendsock.sendto(b'', ('127.0.0.1', port))
        sendsock.sendto(b'', ('127.0.0.1', port))
        self.tb_rcv.wait()
        sendsock.close()
        recv_data = tuple(dst.data())

        self.assertEqual(expected_result, recv_data)



    def test_003(self):
        port = 65530

        udp_rcv = blocks.udp_source(gr.sizeof_float, '0.0.0.0', 0, eof=False)
        rcv_port = udp_rcv.get_port()

        udp_snd = blocks.udp_sink(gr.sizeof_float, '127.0.0.1', port)
        udp_snd.connect('127.0.0.1', rcv_port)

        n_data = 16
        src_data = [float(x) for x in range(n_data)]
        expected_result = tuple(src_data)
        src = blocks.vector_source_f(src_data)
        dst = blocks.vector_sink_f()

        self.tb_snd.connect(src, udp_snd)
        self.tb_rcv.connect(udp_rcv, dst)

        self.tb_rcv.start()
        time.sleep(0.1)
        self.tb_snd.run()
        udp_snd.disconnect()
        self.timeout = False
        q = Timer(2.0,self.stop_rcv)
        q.start()
        self.tb_rcv.wait()
        q.cancel()

        result_data = dst.data()
        self.assertEqual(expected_result, result_data)
        self.assertTrue(self.timeout)  # source ignores EOF?

    def stop_rcv(self):
        self.timeout = True
        self.tb_rcv.stop()
        #print "tb_rcv stopped by Timer"

if __name__ == '__main__':
    gr_unittest.run(test_udp_sink_source, "test_udp_sink_source.xml")

