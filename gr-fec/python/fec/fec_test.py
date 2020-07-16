#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio.fec.bitflip import read_bitlist
from gnuradio import gr, blocks, analog
import math
import sys

if "gnuradio.digital" in sys.modules:
    digital = sys.modules["gnuradio.digital"]
else:
    from gnuradio import digital

from .extended_encoder import extended_encoder
from .extended_decoder import extended_decoder

class fec_test(gr.hier_block2):

    def __init__(self, generic_encoder=0, generic_decoder=0, esno=0,
                 samp_rate=3200000, threading="capillary", puncpat='11',
                 seed=0):
        gr.hier_block2.__init__(self, "fec_test",
                                gr.io_signature(1, 1, gr.sizeof_char*1),
                                gr.io_signature(2, 2, gr.sizeof_char*1))

        self.generic_encoder = generic_encoder
        self.generic_decoder = generic_decoder
        self.esno = esno
        self.samp_rate = samp_rate
        self.threading = threading
        self.puncpat = puncpat

        self.map_bb = digital.map_bb(([-1, 1]))
        self.b2f = blocks.char_to_float(1, 1)

        self.unpack8 = blocks.unpack_k_bits_bb(8)
        self.pack8 = blocks.pack_k_bits_bb(8)

        self.encoder = extended_encoder(encoder_obj_list=generic_encoder,
                                        threading=threading,
                                        puncpat=puncpat)

        self.decoder = extended_decoder(decoder_obj_list=generic_decoder,
                                        threading=threading,
                                        ann=None, puncpat=puncpat,
                                        integration_period=10000, rotator=None)

        noise = math.sqrt((10.0**(-esno / 10.0))/2.0)
        #self.fastnoise = analog.fastnoise_source_f(analog.GR_GAUSSIAN, noise, seed, 8192)
        self.fastnoise = analog.noise_source_f(analog.GR_GAUSSIAN, noise, seed)
        self.addnoise = blocks.add_ff(1)

        # Send packed input directly to the second output
        self.copy_packed = blocks.copy(gr.sizeof_char)
        self.connect(self, self.copy_packed)
        self.connect(self.copy_packed, (self, 1))

        # Unpack inputl encode, convert to +/-1, add noise, decode, repack
        self.connect(self, self.unpack8)
        self.connect(self.unpack8, self.encoder)
        self.connect(self.encoder, self.map_bb)
        self.connect(self.map_bb, self.b2f)
        self.connect(self.b2f, (self.addnoise, 0))
        self.connect(self.fastnoise, (self.addnoise,1))
        self.connect(self.addnoise, self.decoder)
        self.connect(self.decoder, self.pack8)
        self.connect(self.pack8, (self, 0))


    def get_generic_encoder(self):
        return self.generic_encoder

    def set_generic_encoder(self, generic_encoder):
        self.generic_encoder = generic_encoder

    def get_generic_decoder(self):
        return self.generic_decoder

    def set_generic_decoder(self, generic_decoder):
        self.generic_decoder = generic_decoder

    def get_esno(self):
        return self.esno

    def set_esno(self, esno):
        self.esno = esno

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_threading(self):
        return self.threading

    def set_threading(self, threading):
        self.threading = threading

    def get_puncpat(self):
        return self.puncpat

    def set_puncpat(self, puncpat):
        self.puncpat = puncpat
