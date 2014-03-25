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
import math

class fec_raw_test(gr.hier_block2):

    def __init__(self, esno=0, samp_rate=3200000):
        gr.hier_block2.__init__(
            self, "fec_raw_test",
            gr.io_signature(1, 1, gr.sizeof_char*1),
            gr.io_signature(2, 2, gr.sizeof_char*1))

        self.esno = esno
        self.samp_rate = samp_rate

        self.gr_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.gr_unpacked_to_packed_xx_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.char_to_float = blocks.char_to_float()
        self.float_to_char = blocks.float_to_char()
        self.gr_throttle_0 = blocks.throttle(gr.sizeof_char*1, samp_rate)

        noise = math.sqrt((10.0**(-esno/10.0))/2.0)
        self.fastnoise = analog.fastnoise_source_f(analog.GR_GAUSSIAN, noise, 0, 8192)
        self.gaussnoise_ff_0 = blocks.add_ff(1)
        self.connect(self.fastnoise, (self.gaussnoise_ff_0,1))

        self.connect((self.gr_unpacked_to_packed_xx_0_0, 0), (self, 0))
        self.connect((self.gr_unpacked_to_packed_xx_0, 0), (self, 1))
        self.connect((self.gr_throttle_0, 0), (self.gr_unpacked_to_packed_xx_0, 0))
        self.connect((self.gaussnoise_ff_0, 0), (self.float_to_char, 0))
        self.connect((self.float_to_char, 0), (self.gr_unpacked_to_packed_xx_0_0, 0))
        self.connect(self, (self.gr_throttle_0, 0))
        self.connect((self.gr_throttle_0, 0), (self.char_to_float, 0))
        self.connect((self.char_to_float, 0), (self.gaussnoise_ff_0, 0))

    def get_generic_encoder(self):
        return None

    def set_generic_encoder(self, generic_encoder):
        pass

    def get_generic_decoder(self):
        return None

    def set_generic_decoder(self, generic_decoder):
        pass

    def get_esno(self):
        return self.esno

    def set_esno(self, esno):
        self.esno = esno

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_threading(self):
        return None

    def set_threading(self, threading):
        pass

    def get_puncpat(self):
        return None

    def set_puncpat(self, puncpat):
        pass
