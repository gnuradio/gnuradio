#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2022 Josh Morman
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, pdu, streamops
import numpy as np
import pmtf as pmt
import time


class qa_pdu_basic (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_001_pdu_to_stream(self):
        p2s = pdu.pdu_to_stream_c()
        snk = blocks.vector_sink_c()

        self.tb.connect((p2s, 0), (snk, 0))

        in_data = np.array(list(range(1000)), dtype=np.float32) + \
            np.array(list(range(1000)), dtype=np.float32)*1j
        in_pdu = pmt.pmt({"meta": pmt.pmt({}), "data": pmt.pmt(in_data)})

        self.tb.start()

        msg_port = p2s.get_message_port("pdus")

        for _ in range(3):
            msg_port.post(in_pdu)

        time.sleep(0.25)

        self.tb.stop()
        self.tb.wait()

        self.assertTrue(np.array_equal(snk.data(), 3*(in_data.tolist())))

    def test_002_pdu_to_stream_f(self):
        p2s = pdu.pdu_to_stream_f()
        snk = blocks.vector_sink_f()

        self.tb.connect((p2s, 0), (snk, 0))
        in_data = np.array(list(range(1000)), dtype=np.float32)
        in_pdu = pmt.pmt({"meta": pmt.pmt({}), "data": pmt.pmt(in_data)})
        self.tb.start()

        msg_port = p2s.get_message_port("pdus")
        for _ in range(3):
            msg_port.post(in_pdu)

        time.sleep(0.25)

        self.tb.stop()
        self.tb.wait()

        self.assertTrue(np.array_equal(snk.data(), 3*(in_data.tolist())))


    def test_003_stream_to_stream(self):
        nsamps = 1000
        pktsize = 100
        vec = list(range(pktsize))
        src = blocks.vector_source_f(vec, repeat=True)
        p2s = pdu.pdu_to_stream_f()
        s2p = pdu.stream_to_pdu_f(100)
        hd = streamops.head(nsamps)
        snk = blocks.vector_sink_f()

        self.tb.connect([src, s2p])
        self.tb.connect([p2s, hd, snk])
        self.tb.connect((s2p, "pdus"), (p2s, "pdus"))
        self.tb.start()
        self.tb.wait()

        self.assertSequenceEqual(snk.data(), (nsamps//pktsize)*vec)


    def test_004_pdu_to_pdu(self):
        nsamps = 1000 # number of samples in the initial pdus
        pktsize = 100 # pkt size of the pdus from stream_to_pdu
        npdu = 17

        vec = list(range(pktsize))
        in_data = np.array( (nsamps // pktsize)*vec, dtype=np.float32)
        in_pdu = pmt.pmt({"meta": pmt.pmt({}), "data": pmt.pmt(in_data)})

        
        p2s = pdu.pdu_to_stream_f()
        s2p = pdu.stream_to_pdu_f(100)
        snk = blocks.message_debug()

        self.tb.connect([p2s, s2p])
        self.tb.connect((s2p, "pdus"), (snk, "store"))


        self.tb.start()

        msg_port = p2s.get_message_port("pdus")
        for _ in range(npdu):
            msg_port.post(in_pdu)

        time.sleep(0.5)
        self.tb.stop()

        for ii in range((nsamps // pktsize)*npdu):
            rxpdu = snk.get_message(ii)
            rxvec = pmt.map(rxpdu)["data"]()
            self.assertTrue(np.array_equal(vec, rxvec ))




if __name__ == '__main__':
    gr_unittest.run(qa_pdu_basic)
