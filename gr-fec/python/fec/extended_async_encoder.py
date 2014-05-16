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
from bitflip import read_bitlist
import weakref

class extended_async_encoder(gr.hier_block2):
    def __init__(self, encoder_obj_list, puncpat=None):
        gr.hier_block2.__init__(self, "extended_async_encoder",
                                gr.io_signature(0, 0, 0),
                                gr.io_signature(0, 0, 0))

        # Set us up as a message passing block
        self.message_port_register_hier_in('in')
        self.message_port_register_hier_out('out')

        self.puncpat=puncpat

        # If it's a list of encoders, take the first one, unless it's
        # a list of lists of encoders.
        if(type(encoder_obj_list) == list):
            # This block doesn't handle parallelism of > 1
            if(type(encoder_obj_list[0]) == list):
                gr.log.info("fec.extended_encoder: Parallelism must be 0 or 1.")
                raise AttributeError

            encoder_obj = encoder_obj_list[0]

        # Otherwise, just take it as is
        else:
            encoder_obj = encoder_obj_list

        self.encoder = fec.async_encoder(encoder_obj)

        #self.puncture = None
        #if self.puncpat != '11':
        #    self.puncture = fec.puncture_bb(len(puncpat), read_bitlist(puncpat), 0)

        self.msg_connect(weakref.proxy(self), "in", self.encoder, "in")

        #if(self.puncture):
        #    self.msg_connect(self.encoder, "out", self.puncture, "in")
        #    self.msg_connect(self.puncture, "out", weakref.proxy(self), "out")
        #else:
        #    self.msg_connect(self.encoder, "out", weakref.proxy(self), "out")
        self.msg_connect(self.encoder, "out", weakref.proxy(self), "out")
