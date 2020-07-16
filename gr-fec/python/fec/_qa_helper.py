#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#



import numpy

from gnuradio import gr, blocks
# Must use absolute import here because this file is not installed as part
# of the module
from gnuradio.fec import extended_encoder
from gnuradio.fec import extended_decoder


class map_bb(gr.sync_block):
    def __init__(self, bitmap):
        gr.sync_block.__init__(
            self,
            name = "map_bb",
            in_sig = [numpy.int8],
            out_sig = [numpy.int8])
        self.bitmap = bitmap

    def work(self, input_items, output_items):
        output_items[0][:] = [self.bitmap[x] for x in input_items[0]]
        return len(output_items[0])


class _qa_helper(gr.top_block):

    def __init__(self, data_size, enc, dec, threading):
        gr.top_block.__init__(self, "_qa_helper")

        self.puncpat = puncpat = '11'

        self.enc = enc
        self.dec = dec
        self.data_size = data_size
        self.threading = threading

        self.ext_encoder = extended_encoder(enc, threading=self.threading, puncpat=self.puncpat)
        self.ext_decoder= extended_decoder(dec, threading=self.threading, ann=None,
                                           puncpat=self.puncpat, integration_period=10000)

        self.src = blocks.vector_source_b(data_size*[0, 1, 2, 3, 5, 7, 9, 13, 15, 25, 31, 45, 63, 95, 127], False)
        self.unpack = blocks.unpack_k_bits_bb(8)
        self.map = map_bb([-1, 1])
        self.to_float = blocks.char_to_float(1)
        self.snk_input = blocks.vector_sink_b()
        self.snk_output = blocks.vector_sink_b()

        self.connect(self.src, self.unpack, self.ext_encoder)
        self.connect(self.ext_encoder, self.map, self.to_float)
        self.connect(self.to_float, self.ext_decoder)
        self.connect(self.unpack, self.snk_input)
        self.connect(self.ext_decoder, self.snk_output)

if __name__ == '__main__':
    frame_size = 30
    enc = fec.dummy_encoder_make(frame_size*8)
    #enc = fec.repetition_encoder_make(frame_size*8, 3)
    dec = fec.dummy_decoder.make(frame_size*8)

    tb = _qa_helper(10*frame_size, enc, dec, None)
    tb.run()

    errs = 0
    for i,o in zip(tb.snk_input.data(), tb.snk_output.data()):
        if i-o != 0:
            errs += 1

    if errs == 0:
        print("Decoded properly")
    else:
        print("Problem Decoding")
