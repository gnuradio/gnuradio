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

from gnuradio import gr, blocks
import fec_swig as fec

class threaded_encoder(gr.hier_block2):
    def __init__(self, encoder_list_0, input_size, output_size):
        gr.hier_block2.__init__(
            self, "Threaded Encoder",
            gr.io_signature(1, 1, input_size*1),
            gr.io_signature(1, 1, output_size*1))

        self.encoder_list_0 = encoder_list_0

        self.fec_deinterleave_0 = blocks.deinterleave(input_size,
                                                      fec.get_encoder_input_size(encoder_list_0[0]))

        self.generic_encoders_0 = [];
        for i in range(len(encoder_list_0)):
            self.generic_encoders_0.append(fec.encoder(encoder_list_0[i],
                                                       input_size, output_size))

        self.fec_interleave_0 = blocks.interleave(output_size,
                                               fec.get_encoder_output_size(encoder_list_0[0]))

        for i in range(len(encoder_list_0)):
            self.connect((self.fec_deinterleave_0, i), (self.generic_encoders_0[i], 0))

        for i in range(len(encoder_list_0)):
            self.connect((self.generic_encoders_0[i], 0), (self.fec_interleave_0, i))

        self.connect((self, 0), (self.fec_deinterleave_0, 0))
        self.connect((self.fec_interleave_0, 0), (self, 0))

    def get_encoder_list_0(self):
        return self.encoder_list_0

    def set_encoder_list_0(self, encoder_list_0):
        self.encoder_list_0 = encoder_list_0
