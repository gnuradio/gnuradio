#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import weakref

from gnuradio import gr

from . import fec_swig as fec
from .bitflip import read_bitlist


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
