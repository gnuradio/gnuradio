#!/usr/bin/env python
#
# Copyright 2004,2010,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math
import os

from gnuradio import gr, gr_unittest, trellis, digital, analog, blocks

fsm_args = {"awgn1o2_4": [2, 4, 4,
                          [0, 2, 0, 2, 1, 3, 1, 3],
                          [0, 3, 3, 0, 1, 2, 2, 1],
                          ],
            "rep2": [2, 1, 4, [0, 0], [0, 3]],
            "nothing": [2, 1, 2, [0, 0], [0, 1]],
            }

constells = {2: digital.constellation_bpsk(),
             4: digital.constellation_qpsk(),
             }


class test_trellis (gr_unittest.TestCase):

    def test_001_fsm(self):
        f = trellis.fsm(*fsm_args["awgn1o2_4"])
        self.assertEqual(
            fsm_args["awgn1o2_4"], [
                f.I(), f.S(), f.O(), f.NS(), f.OS()])

    def test_002_fsm(self):
        f = trellis.fsm(*fsm_args["awgn1o2_4"])
        g = trellis.fsm(f)
        self.assertEqual((g.I(), g.S(), g.O(), g.NS(), g.OS()),
                         (f.I(), f.S(), f.O(), f.NS(), f.OS()))

    def test_003_fsm(self):
        # FIXME: no file "awgn1o2_4.fsm"
        #f = trellis.fsm("awgn1o2_4.fsm")
        # self.assertEqual(fsm_args["awgn1o2_4"],(f.I(),f.S(),f.O(),f.NS(),f.OS()))
        pass

    def test_004_fsm(self):
        """ Test to make sure fsm works with a single state fsm."""
        # Just checking that it initializes properly.
        f = trellis.fsm(*fsm_args["rep2"])

    def test_001_interleaver(self):
        K = 5
        IN = [1, 2, 3, 4, 0]
        DIN = [4, 0, 1, 2, 3]
        i = trellis.interleaver(K, IN)
        self.assertEqual((K, IN, DIN), (i.K(), i.INTER(), i.DEINTER()))

    def test_001_viterbi(self):
        """
        Runs some coding/decoding tests with a few different FSM
        specs.
        """
        for name, args in list(fsm_args.items()):
            constellation = constells[args[2]]
            fsms = trellis.fsm(*args)
            noise = 0.1
            tb = trellis_tb(constellation, fsms, noise)
            tb.run()
            # Make sure all packets successfully transmitted.
            self.assertEqual(tb.dst.ntotal(), tb.dst.nright())


class trellis_tb(gr.top_block):
    """
    A simple top block for use testing gr-trellis.
    """

    def __init__(self, constellation, f, N0=0.25, seed=-666):
        """
        constellation - a constellation object used for modulation.
        f - a finite state machine specification used for coding.
        N0 - noise level
        seed - random seed
        """
        super(trellis_tb, self).__init__()
        # packet size in bits (make it multiple of 16 so it can be packed in a
        # short)
        packet_size = 1024 * 16
        # bits per FSM input symbol
        # bits per FSM input symbol
        bitspersymbol = int(round(math.log(f.I()) / math.log(2)))
        # packet size in trellis steps
        K = packet_size // bitspersymbol

        # TX
        src = blocks.lfsr_32k_source_s()
        # packet size in shorts
        src_head = blocks.head(gr.sizeof_short, packet_size // 16)
        # unpack shorts to symbols compatible with the FSM input cardinality
        s2fsmi = blocks.packed_to_unpacked_ss(bitspersymbol, gr.GR_MSB_FIRST)
        # initial FSM state = 0
        enc = trellis.encoder_ss(f, 0)
        mod = digital.chunks_to_symbols_sc(constellation.points(), 1)

        # CHANNEL
        add = blocks.add_cc()
        noise = analog.noise_source_c(
            analog.GR_GAUSSIAN, math.sqrt(
                N0 / 2), seed)

        # RX
        # data preprocessing to generate metrics for Viterbi
        metrics = trellis.constellation_metrics_cf(
            constellation.base(), digital.TRELLIS_EUCLIDEAN)
        # Put -1 if the Initial/Final states are not set.
        va = trellis.viterbi_s(f, K, 0, -1)
        # pack FSM input symbols to shorts
        fsmi2s = blocks.unpacked_to_packed_ss(bitspersymbol, gr.GR_MSB_FIRST)
        # check the output
        self.dst = blocks.check_lfsr_32k_s()

        self.connect(src, src_head, s2fsmi, enc, mod)
        self.connect(mod, (add, 0))
        self.connect(noise, (add, 1))
        self.connect(add, metrics, va, fsmi2s, self.dst)


if __name__ == '__main__':
    gr_unittest.run(test_trellis)
