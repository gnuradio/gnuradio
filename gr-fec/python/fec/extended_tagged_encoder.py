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
from bitflip import read_bitlist

class extended_tagged_encoder(gr.hier_block2):
    def __init__(self, encoder_obj_list, puncpat=None, lentagname=None, mtu=1500):
        gr.hier_block2.__init__(self, "extended_tagged_encoder",
                                gr.io_signature(1, 1, gr.sizeof_char),
                                gr.io_signature(1, 1, gr.sizeof_char))

        self.blocks=[]
        self.puncpat=puncpat

        # If it's a list of encoders, take the first one, unless it's
        # a list of lists of encoders.
        if(type(encoder_obj_list) == list):
            # This block doesn't handle parallelism of > 1
            # We could just grab encoder [0][0], but we don't want to encourage this.
            if(type(encoder_obj_list[0]) == list):
                gr.log.info("fec.extended_tagged_encoder: Parallelism must be 0 or 1.")
                raise AttributeError

            encoder_obj = encoder_obj_list[0]

        # Otherwise, just take it as is
        else:
            encoder_obj = encoder_obj_list

        # If lentagname is None, fall back to using the non tagged
        # stream version
        if type(lentagname) == str:
            if(lentagname.lower() == 'none'):
                lentagname = None

        if fec.get_encoder_input_conversion(encoder_obj) == "pack":
            self.blocks.append(blocks.pack_k_bits_bb(8))

        if(not lentagname):
            self.blocks.append(fec.encoder(encoder_obj,
                                           gr.sizeof_char,
                                           gr.sizeof_char))
        else:
            self.blocks.append(fec.tagged_encoder(encoder_obj,
                                                  gr.sizeof_char,
                                                  gr.sizeof_char,
                                                  lentagname, mtu))

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
