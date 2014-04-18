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

from gnuradio import gr

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

        if self.puncpat != '11':
            self.blocks.append(fec.puncture_bb(0, read_bitlist(puncpat),
                                               puncpat.count('0'), len(puncpat)))

        # Connect the input to the encoder and the output to the
        # puncture if used or the encoder if not.
        self.connect((self, 0), (self.blocks[0], 0));
        self.connect((self.blocks[-1], 0), (self, 0));

        # If using the puncture block, add it into the flowgraph after
        # the encoder.
        for i in range(len(self.blocks) - 1):
            self.connect((self.blocks[i], 0), (self.blocks[i+1], 0));

