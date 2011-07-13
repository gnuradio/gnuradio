#!/usr/bin/env python
#
# Copyright 2004,2010 Free Software Foundation, Inc.
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

import math


from gnuradio import gr, gr_unittest, blks2
# It's pretty ugly that we can't import trellis from gnuradio in this test
# but because it runs on the non-installed python code it's all a mess.
import trellis

import os
import digital_swig

fsm_args = {"awgn1o2_4": (2, 4, 4,
                          (0, 2, 0, 2, 1, 3, 1, 3),
                          (0, 3, 3, 0, 1, 2, 2, 1),
                          ),
            "rep2": (2, 1, 4, (0, 0), (0, 3)),
            "nothing": (2, 1, 2, (0, 0), (0, 1)),
            }

constells = {2: digital_swig.constellation_bpsk(),
             4: digital_swig.constellation_qpsk(),
             }

class test_trellis (gr_unittest.TestCase):

    def test_001_fsm (self):
        f = trellis.fsm(*fsm_args["awgn1o2_4"])
        self.assertEqual(fsm_args["awgn1o2_4"],(f.I(),f.S(),f.O(),f.NS(),f.OS()))

    def test_002_fsm (self):
        f = trellis.fsm(*fsm_args["awgn1o2_4"])
        g = trellis.fsm(f)
        self.assertEqual((g.I(),g.S(),g.O(),g.NS(),g.OS()),(f.I(),f.S(),f.O(),f.NS(),f.OS()))

    def test_003_fsm (self):
        # FIXME: no file "awgn1o2_4.fsm"
        #f = trellis.fsm("awgn1o2_4.fsm")
        #self.assertEqual(fsm_args["awgn1o2_4"],(f.I(),f.S(),f.O(),f.NS(),f.OS()))
        pass

    def test_004_fsm(self):
        """ Test to make sure fsm works with a single state fsm."""
        # Just checking that it initializes properly.
        f = trellis.fsm(*fsm_args["rep2"])

    def test_001_interleaver (self):
        K = 5
        IN = (1,2,3,4,0)
        DIN = (4,0,1,2,3)
        i = trellis.interleaver(K,IN)
        self.assertEqual((K,IN,DIN),(i.K(),i.INTER(),i.DEINTER()))

    def test_001_viterbi(self):
        """
        Runs some coding/decoding tests with a few different FSM
        specs.
        """
        for name, args in fsm_args.items():
            constellation = constells[args[2]]
            fsms = trellis.fsm(*args)
            noise = 0.1
            tb = trellis_tb(constellation, fsms, noise)
            tb.run()
            # Make sure all packets succesfully transmitted.
            self.assertEqual(tb.dst.ntotal(), tb.dst.nright())


class trellis_tb(gr.top_block):
    """
    A simple top block for use testing gr-trellis.
    """
    def __init__(self, constellation, f, N0=0.25, seed=-666L):
        """
        constellation - a constellation object used for modulation.
        f - a finite state machine specification used for coding.
        N0 - noise level
        seed - random seed
        """
        super(trellis_tb, self).__init__()
        # packet size in bits (make it multiple of 16 so it can be packed in a short)
        packet_size = 1024*16
        # bits per FSM input symbol
        bitspersymbol = int(round(math.log(f.I())/math.log(2))) # bits per FSM input symbol
        # packet size in trellis steps
        K = packet_size/bitspersymbol

        # TX
        src = gr.lfsr_32k_source_s()
        # packet size in shorts
        src_head = gr.head (gr.sizeof_short, packet_size/16)
        # unpack shorts to symbols compatible with the FSM input cardinality
        s2fsmi = gr.packed_to_unpacked_ss(bitspersymbol, gr.GR_MSB_FIRST) 
        # initial FSM state = 0
        enc = trellis.encoder_ss(f, 0) 
        mod = gr.chunks_to_symbols_sc(constellation.points(), 1)

        # CHANNEL
        add = gr.add_cc()
        noise = gr.noise_source_c(gr.GR_GAUSSIAN,math.sqrt(N0/2),seed)

        # RX
        # data preprocessing to generate metrics for Viterbi
        metrics = trellis.constellation_metrics_cf(constellation.base(), digital_swig.TRELLIS_EUCLIDEAN) 
        # Put -1 if the Initial/Final states are not set.
        va = trellis.viterbi_s(f, K, 0, -1)
        # pack FSM input symbols to shorts
        fsmi2s = gr.unpacked_to_packed_ss(bitspersymbol, gr.GR_MSB_FIRST) 
        # check the output
        self.dst = gr.check_lfsr_32k_s()
    
        self.connect (src, src_head, s2fsmi, enc, mod)
        self.connect (mod, (add, 0))
        self.connect (noise, (add, 1))
        self.connect (add, metrics, va, fsmi2s, self.dst)


if __name__ == '__main__':
    gr_unittest.run(test_trellis, "test_trellis.xml")
