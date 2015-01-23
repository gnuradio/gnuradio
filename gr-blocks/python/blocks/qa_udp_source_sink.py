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
import os

from threading import Timer

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

    def test_002(self):
        port = 65520

        n_data = 100
        src_data = [float(x) for x in range(n_data)]
        expected_result = tuple(src_data)
        src = blocks.vector_source_f(src_data, False)
        udp_snd = blocks.udp_sink(gr.sizeof_float, 'localhost', port)
        self.tb_snd.connect(src, udp_snd)

        udp_rcv = blocks.udp_source(gr.sizeof_float, 'localhost', port)
        dst = blocks.vector_sink_f()
        self.tb_rcv.connect(udp_rcv, dst)

        self.tb_rcv.start()
        self.tb_snd.run()
        udp_snd.disconnect()
        self.timeout = False
        q = Timer(2.0,self.stop_rcv)
        q.start()
        self.tb_rcv.wait()
        q.cancel()

        result_data = dst.data()
        self.assertEqual(expected_result, result_data)
        self.assert_(not self.timeout)

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
        self.tb_snd.run()
        udp_snd.disconnect()
        self.timeout = False
        q = Timer(2.0,self.stop_rcv)
        q.start()
        self.tb_rcv.wait()
        q.cancel()

        result_data = dst.data()
        self.assertEqual(expected_result, result_data)
        self.assert_(self.timeout)  # source ignores EOF?

    def stop_rcv(self):
        self.timeout = True
        self.tb_rcv.stop()
        #print "tb_rcv stopped by Timer"

if __name__ == '__main__':
    gr_unittest.run(test_udp_sink_source, "test_udp_sink_source.xml")

