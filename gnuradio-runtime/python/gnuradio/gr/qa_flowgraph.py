#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
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

import time
import pmt
from gnuradio import gr, gr_unittest, blocks

class test_flowgraph (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_000(self):

        self.tb.start()
        self.tb.lock()

        rem = blocks.pdu_remove(pmt.intern('foo'))
        dbg = blocks.message_debug()
        self.tb.msg_connect((rem, 'pdus'), (dbg, 'store'))

        self.tb.unlock()

        msg = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(3, (1, 2, 3)))
        rem.to_basic_block()._post(pmt.intern('pdus'), msg)
        time.sleep(0.2)

        self.tb.stop()

        self.assertEqual(dbg.num_messages(), 1)
        data = pmt.u8vector_elements(pmt.cdr(dbg.get_message(0)))
        self.assertEqual((1, 2, 3), data)

if __name__ == '__main__':
    gr_unittest.run(test_flowgraph, 'test_flowgraph.xml')

