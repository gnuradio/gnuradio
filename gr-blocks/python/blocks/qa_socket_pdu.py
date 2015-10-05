#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
import random


class qa_socket_pdu (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        # Test that blocks can be created and destroyed without hanging
        port = str(random.Random().randint(0, 30000) + 10000)
        self.pdu_send = blocks.socket_pdu("UDP_CLIENT", "localhost", port)
        self.pdu_recv = blocks.socket_pdu("UDP_SERVER", "localhost", port)
        self.pdu_send = None
        self.pdu_recv = None

if __name__ == '__main__':
    gr_unittest.run(qa_socket_pdu, "qa_socket_pdu.xml")

