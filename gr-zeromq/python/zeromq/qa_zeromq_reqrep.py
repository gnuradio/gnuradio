#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 202013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks, zeromq
from gnuradio import eng_notation

class qa_zeromq_reqrep (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        print "test_001"
        src_data = [1,2,3,4,5,6,7,8,9,0]*100
        src = blocks.vector_source_c(src_data, False, 1)
        zeromq_rep_sink = zeromq.rep_sink(gr.sizeof_gr_complex, 10, "tcp://127.0.0.1:5555", 0)
        zeromq_req_source = zeromq.req_source(gr.sizeof_gr_complex, 10, "tcp://127.0.0.1:5555", 0)
        sink = blocks.vector_sink_c()
        self.tb.connect(src, zeromq_rep_sink)
        self.tb.connect(zeromq_req_source, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), src_data)

if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_reqrep)
