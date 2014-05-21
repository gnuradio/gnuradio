#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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

from gnuradio import blocks
from gnuradio import gr
import sys, numpy

from extended_encoder import extended_encoder
from extended_decoder import extended_decoder

class map_bb(gr.sync_block):
    def __init__(self, bitmap):
        gr.sync_block.__init__(
            self,
            name = "map_bb",
            in_sig = [numpy.int8],
            out_sig = [numpy.int8])
        self.bitmap = bitmap

    def work(self, input_items, output_items):
        output_items[0][:] = map(lambda x: self.bitmap[x], input_items[0])
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
        print "Decoded properly"
    else:
        print "Problem Decoding"
