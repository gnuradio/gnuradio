#!/usr/bin/env python
# Copyright 2012 Free Software Foundation, Inc.
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
import pmt
import digital_swig as digital
import blocks_swig as blocks
import time

class qa_header_payload_demux (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """ Simplest possible test: put in zeros, then header,
        then payload, trigger signal, try to demux.
        The return signal from the header parser is faked via _post()
        """
        n_zeros = 100
        header = (1, 2, 3)
        payload = tuple(range(17))
        data_signal = (0,) * n_zeros + header + payload
        trigger_signal = [0,] * len(data_signal)
        trigger_signal[n_zeros] = 1

        data_src = blocks.vector_source_f(data_signal, False)
        trigger_src = blocks.vector_source_b(trigger_signal, False)
        hpd = digital.header_payload_demux(
            len(header), 1, 0, "frame_len", "detect", False, gr.sizeof_float
        )
        self.assertEqual(pmt.length(hpd.message_ports_in()), 1)
        header_sink = blocks.vector_sink_f()
        payload_sink = blocks.vector_sink_f()

        self.tb.connect(data_src,    (hpd, 0))
        self.tb.connect(trigger_src, (hpd, 1))
        self.tb.connect((hpd, 0), header_sink)
        self.tb.connect((hpd, 1), payload_sink)
        self.tb.start()
        time.sleep(.2) # Need this, otherwise, the next message is ignored
        hpd.to_basic_block()._post(
                pmt.intern('header_data'),
                pmt.from_long(len(payload))
        )
        while len(payload_sink.data()) < len(payload):
            time.sleep(.2)
        self.tb.stop()
        self.tb.wait()

        self.assertEqual(header_sink.data(),  header)
        self.assertEqual(payload_sink.data(), payload)


if __name__ == '__main__':
    gr_unittest.run(qa_header_payload_demux, "qa_header_payload_demux.xml")

