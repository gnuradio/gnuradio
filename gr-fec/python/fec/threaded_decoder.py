#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import unicode_literals
from gnuradio import gr, blocks
from . import fec_swig as fec


class threaded_decoder(gr.hier_block2):
    def __init__(self, decoder_list_0, input_size, output_size):
        gr.hier_block2.__init__(
            self, "Threaded Decoder",
            gr.io_signature(1, 1, input_size*1),
            gr.io_signature(1, 1, output_size*1))

        self.decoder_list_0 = decoder_list_0

        self.deinterleave_0 = blocks.deinterleave(input_size,
                                                  fec.get_decoder_input_size(decoder_list_0[0]))

        self.generic_decoders_0 = []
        for i in range(len(decoder_list_0)):
            self.generic_decoders_0.append(fec.decoder(decoder_list_0[i],
                                                       input_size, output_size))

        self.interleave_0 = blocks.interleave(output_size,
                                              fec.get_decoder_output_size(decoder_list_0[0]))

        for i in range(len(decoder_list_0)):
            self.connect((self.deinterleave_0, i), (self.generic_decoders_0[i], 0))

        for i in range(len(decoder_list_0)):
            self.connect((self.generic_decoders_0[i], 0), (self.interleave_0, i))


        self.connect((self, 0), (self.deinterleave_0, 0))
        self.connect((self.interleave_0, 0), (self, 0))

    def get_decoder_list_0(self):
        return self.decoder_list_0

    def set_decoder_list_0(self, decoder_list_0):
        self.decoder_list_0 = decoder_list_0
