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

import numpy
from gnuradio import gr, gr_unittest
import digital_swig

class test_ofdm_txrx (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        pass
        #len_tag_key = 'frame_len'
        #n_bytes = 100
        #test_data = [random.randint(0, 255) for x in range(n_bytes)]
        #tx_data, tags = tagged_streams.packets_to_vectors((test_data,), len_tag_key)
        #src = gr.vector_source_b(test_data, False, 1, tags)
        #tx = ofdm_tx(frame_length_tag_key=len_tag_key)
        #rx = ofdm_rx(frame_length_tag_key=len_tag_key)
        #self.assertEqual(tx.sync_word1, rx.sync_word1)
        #self.assertEqual(tx.sync_word2, rx.sync_word2)
        #delay = gr.delay(gr.sizeof_gr_complex, 100)
        #noise = gr.noise_source_c(gr.GR_GAUSSIAN, 0.05)
        #add = gr.add_cc()
        #sink = gr.vector_sink_b()
        ##self.tb.connect(src, tx, add, rx, sink)
        ##self.tb.connect(noise, (add, 1))
        #self.tb.connect(src, tx, gr.null_sink(gr.sizeof_gr_complex))
        #self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(test_ofdm_txrx, "test_ofdm_txrx.xml")

