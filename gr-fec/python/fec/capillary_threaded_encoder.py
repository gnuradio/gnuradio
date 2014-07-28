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
import math

class capillary_threaded_encoder(gr.hier_block2):
    def __init__(self, encoder_list_0, input_size=gr.sizeof_char, output_size=gr.sizeof_char):
        gr.hier_block2.__init__(self, "Capillary Threaded Encoder",
                                gr.io_signature(1, 1, input_size),
                                gr.io_signature(1, 1, output_size))

        self.encoder_list_0 = encoder_list_0

        check = math.log10(len(self.encoder_list_0)) / math.log10(2.0)
        if(abs(check - int(check)) > 0.0):
            gr.log.info("fec.capillary_threaded_encoder: number of encoders must be a power of 2.")
            raise AttributeError

        self.deinterleaves_0 = [];
        for i in range(int(math.log(len(encoder_list_0), 2))):
            for j in range(int(math.pow(2, i))):
                self.deinterleaves_0.append(blocks.deinterleave(input_size,
                                                                fec.get_encoder_input_size(encoder_list_0[0])))

       	self.generic_encoders_0 = [];
        for i in range(len(encoder_list_0)):
            self.generic_encoders_0.append(fec.encoder(encoder_list_0[i],
                                                       input_size, output_size))

        self.interleaves_0 = [];
        for i in range(int(math.log(len(encoder_list_0), 2))):
            for j in range(int(math.pow(2, i))):
                self.interleaves_0.append(blocks.interleave(output_size,
                                                            fec.get_encoder_output_size(encoder_list_0[0])))

        rootcount = 0;
        branchcount = 1;
        for i in range(int(math.log(len(encoder_list_0), 2)) - 1):
            for j in range(int(math.pow(2, i))):
                self.connect((self.deinterleaves_0[rootcount], 0), (self.deinterleaves_0[branchcount], 0))
                self.connect((self.deinterleaves_0[rootcount], 1), (self.deinterleaves_0[branchcount + 1], 0))
                rootcount += 1;
                branchcount += 2;

        codercount = 0;
        for i in range(len(encoder_list_0)/2):
            self.connect((self.deinterleaves_0[rootcount], 0), (self.generic_encoders_0[codercount], 0))
            self.connect((self.deinterleaves_0[rootcount], 1), (self.generic_encoders_0[codercount + 1], 0))
            rootcount += 1;
            codercount += 2;


        rootcount = 0;
        branchcount = 1;
        for i in range(int(math.log(len(encoder_list_0), 2)) - 1):
            for j in range(int(math.pow(2, i))):
                self.connect((self.interleaves_0[branchcount], 0), (self.interleaves_0[rootcount], 0))
                self.connect((self.interleaves_0[branchcount + 1], 0), (self.interleaves_0[rootcount], 1))
                rootcount += 1;
                branchcount += 2;


        codercount = 0;
        for i in range(len(encoder_list_0)/2):
            self.connect((self.generic_encoders_0[codercount], 0), (self.interleaves_0[rootcount], 0))
            self.connect((self.generic_encoders_0[codercount + 1], 0), (self.interleaves_0[rootcount], 1))
            rootcount += 1;
            codercount += 2;

       	if((len(self.encoder_list_0)) > 1):
            self.connect((self, 0), (self.deinterleaves_0[0], 0))
            self.connect((self.interleaves_0[0], 0), (self, 0))
        else:
            self.connect((self, 0), (self.generic_encoders_0[0], 0))
            self.connect((self.generic_encoders_0[0], 0), (self, 0))

    def get_encoder_list_0(self):
        return self.encoder_list_0

    def set_encoder_list_0(self, encoder_list_0):
        self.encoder_list_0 = encoder_list_0
