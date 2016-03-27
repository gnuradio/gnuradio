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
import numpy

from fec_test import fec_test

class bercurve_generator(gr.hier_block2):

    def __init__(self, encoder_list, decoder_list, esno=numpy.arange(0.0, 3.0, .25),
                 samp_rate=3200000, threading='capillary', puncpat='11', seed=0):
        gr.hier_block2.__init__(
            self, "ber_curve_generator",
            gr.io_signature(0, 0, 0),
            gr.io_signature(len(esno) * 2, len(esno) * 2, gr.sizeof_char*1))

        self.esno = esno
        self.samp_rate = samp_rate
        self.encoder_list = encoder_list
        self.decoder_list = decoder_list
        self.puncpat = puncpat

        self.random_gen_b_0 = blocks.vector_source_b(map(int, numpy.random.randint(0, 256, 100000)), True)
        self.deinterleave = blocks.deinterleave(gr.sizeof_char*1)
        self.connect(self.random_gen_b_0, self.deinterleave)
        self.ber_generators = []

        # FIXME It would be good to check that the encoder_list and
        # decoder_list have parallelism set to > 0. If parallelism
        # is set to 0, a map isn't passed and an indexing error is
        # thrown on line 53 or 54 below.

        for i in range(0, len(esno)):
            ber_generator_temp = fec_test(
                generic_encoder=encoder_list[i],
                generic_decoder=decoder_list[i],
                esno=esno[i],
                samp_rate=samp_rate,
                threading=threading,
                puncpat=puncpat,
                seed=seed)
            self.ber_generators.append(ber_generator_temp);

        for i in range(0, len(esno)):
            self.connect((self.deinterleave, i), (self.ber_generators[i]))
            self.connect((self.ber_generators[i], 0), (self, i*2));
            self.connect((self.ber_generators[i], 1), (self, i*2 + 1));

    def get_esno(self):
        return self.esno

    def set_esno(self, esno):
        self.esno = esno
        self.ber_generator_0.set_esno(self.esno)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.ber_generator_0.set_samp_rate(self.samp_rate)

    def get_encoder_list(self):
        return self.encoder_list

    def set_encoder_list(self, encoder_list):
        self.encoder_list = encoder_list
        self.ber_generator_0.set_generic_encoder(self.encoder_list)

    def get_decoder_list(self):
        return self.decoder_list

    def set_decoder_list(self, decoder_list):
        self.decoder_list = decoder_list
        self.ber_generator_0.set_generic_decoder(self.decoder_list)

    def get_puncpat(self):
        return self.puncpat

    def set_puncpat(self, puncpat):
        self.puncpat = puncpat
