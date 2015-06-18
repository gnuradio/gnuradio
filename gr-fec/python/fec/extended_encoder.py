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
from threaded_encoder import threaded_encoder
from capillary_threaded_encoder import capillary_threaded_encoder
from bitflip import read_bitlist

class extended_encoder(gr.hier_block2):
    def __init__(self, encoder_obj_list, threading, puncpat=None):
        gr.hier_block2.__init__(self, "extended_encoder",
                                gr.io_signature(1, 1, gr.sizeof_char),
                                gr.io_signature(1, 1, gr.sizeof_char))

        self.blocks=[]
        self.puncpat=puncpat

        if(type(encoder_obj_list) == list):
            if(type(encoder_obj_list[0]) == list):
                gr.log.info("fec.extended_encoder: Parallelism must be 1.")
                raise AttributeError
        else:
            # If it has parallelism of 0, force it into a list of 1
            encoder_obj_list = [encoder_obj_list,]

        if fec.get_encoder_input_conversion(encoder_obj_list[0]) == "pack":
            self.blocks.append(blocks.pack_k_bits_bb(8))

        if threading == 'capillary':
            self.blocks.append(capillary_threaded_encoder(encoder_obj_list,
                                                          gr.sizeof_char,
                                                          gr.sizeof_char))
        elif threading == 'ordinary':
            self.blocks.append(threaded_encoder(encoder_obj_list,
                                                gr.sizeof_char,
                                                gr.sizeof_char))
        else:
            self.blocks.append(fec.encoder(encoder_obj_list[0],
                                           gr.sizeof_char,
                                           gr.sizeof_char))

        if fec.get_encoder_output_conversion(encoder_obj_list[0]) == "packed_bits":
            self.blocks.append(blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST))

        if self.puncpat != '11':
            self.blocks.append(fec.puncture_bb(len(puncpat), read_bitlist(puncpat), 0))

        # Connect the input to the encoder and the output to the
        # puncture if used or the encoder if not.
        self.connect((self, 0), (self.blocks[0], 0));
        self.connect((self.blocks[-1], 0), (self, 0));

        # If using the puncture block, add it into the flowgraph after
        # the encoder.
        for i in range(len(self.blocks) - 1):
            self.connect((self.blocks[i], 0), (self.blocks[i+1], 0));
