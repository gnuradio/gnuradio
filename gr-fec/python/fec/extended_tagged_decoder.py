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
from bitflip import *
import sys

if sys.modules.has_key("gnuradio.digital"):
    digital = sys.modules["gnuradio.digital"]
else:
    from gnuradio import digital

class extended_tagged_decoder(gr.hier_block2):

#solution to log_(1-2*t)(1-2*.0335) = 1/taps where t is thresh (syndrome density)
#for i in numpy.arange(.1, .499, .01):
    #print str(log((1-(2 * .035)), (1-(2 * i)))) + ':' + str(i);
    garbletable = {
        0.310786835319:0.1,
        0.279118162802:0.11,
        0.252699589071:0.12,
        0.230318516016:0.13,
        0.211108735347:0.14,
        0.194434959095:0.15,
        0.179820650401:0.16,
        0.166901324951:0.17,
        0.15539341766:0.18,
        0.145072979886:0.19,
        0.135760766313:0.2,
        0.127311581396:0.21,
        0.119606529806:0.22,
        0.112547286766:0.23,
        0.106051798775:0.24,
        0.10005101381:0.25,
        0.0944863633098:0.26,
        0.0893078003966:0.27,
        0.084472254501:0.28,
        0.0799424008658:0.29,
        0.0756856701944:0.3,
        0.0716734425668:0.31,
        0.0678803831565:0.32,
        0.0642838867856:0.33,
        0.0608636049994:0.34,
        0.0576010337489:0.35,
        0.0544791422522:0.36,
        0.0514820241933:0.37,
        0.0485945507251:0.38,
        0.0458019998183:0.39,
        0.0430896262596:0.4,
        0.0404421166935:0.41,
        0.0378428350972:0.42,
        0.0352726843274:0.43,
        0.0327082350617:0.44,
        0.0301183562535:0.45,
        0.0274574540266:0.46,
        0.0246498236897:0.47,
        0.0215448131298:0.48,
        0.0177274208353:0.49,
    }

    def __init__(self, decoder_obj_list, ann=None, puncpat='11',
                 integration_period=10000, flush=None, rotator=None, lentagname=None,
                 mtu=1500):
        gr.hier_block2.__init__(self, "extended_decoder",
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(1, 1, gr.sizeof_char))
        self.blocks=[]
        self.ann=ann
        self.puncpat=puncpat
        self.flush=flush

        if(type(decoder_obj_list) == list):
            # This block doesn't handle parallelism of > 1
            # We could just grab encoder [0][0], but we don't want to encourage this.
            if(type(decoder_obj_list[0]) == list):
                gr.log.info("fec.extended_tagged_decoder: Parallelism must be 1.")
                raise AttributeError

            decoder_obj = decoder_obj_list[0]

        # Otherwise, just take it as is
        else:
            decoder_obj = decoder_obj_list

        # If lentagname is None, fall back to using the non tagged
        # stream version
        if type(lentagname) == str:
            if(lentagname.lower() == 'none'):
                lentagname = None

        message_collector_connected=False

        ##anything going through the annihilator needs shifted, uchar vals
        if fec.get_decoder_input_conversion(decoder_obj) == "uchar" or \
           fec.get_decoder_input_conversion(decoder_obj) == "packed_bits":
            self.blocks.append(blocks.multiply_const_ff(48.0))

        if fec.get_shift(decoder_obj) != 0.0:
            self.blocks.append(blocks.add_const_ff(fec.get_shift(decoder_obj)))
        elif fec.get_decoder_input_conversion(decoder_obj) == "packed_bits":
            self.blocks.append(blocks.add_const_ff(128.0))

        if fec.get_decoder_input_conversion(decoder_obj) == "uchar" or \
           fec.get_decoder_input_conversion(decoder_obj) == "packed_bits":
            self.blocks.append(blocks.float_to_uchar());

        const_index = 0; #index that corresponds to mod order for specinvert purposes

        if not self.flush:
            flush = 10000;
        else:
            flush = self.flush;
        if self.ann: #ann and puncpat are strings of 0s and 1s
            cat = fec.ULLVector();
            for i in fec.read_big_bitlist(ann):
                cat.append(i);

            synd_garble = .49
            idx_list = self.garbletable.keys()
            idx_list.sort()
            for i in idx_list:
                if 1.0/self.ann.count('1') >= i:
                    synd_garble = self.garbletable[i]
            print 'using syndrom garble threshold ' + str(synd_garble) + 'for conv_bit_corr_bb'
            print 'ceiling: .0335 data garble rate'
            self.blocks.append(fec.conv_bit_corr_bb(cat, len(puncpat) - puncpat.count('0'),
                                           len(ann), integration_period, flush, synd_garble))

        if self.puncpat != '11':
            self.blocks.append(fec.depuncture_bb(len(puncpat), read_bitlist(puncpat), 0))

        if fec.get_decoder_input_conversion(decoder_obj) == "packed_bits":
            self.blocks.append(blocks.uchar_to_float())
            self.blocks.append(blocks.add_const_ff(-128.0))
            self.blocks.append(digital.binary_slicer_fb())
            self.blocks.append(blocks.unpacked_to_packed_bb(1,0))

        else:
            if(not lentagname):
                self.blocks.append(fec.decoder(decoder_obj,
                                               fec.get_decoder_input_item_size(decoder_obj),
                                               fec.get_decoder_output_item_size(decoder_obj)))
            else:
                self.blocks.append(fec.tagged_decoder(decoder_obj,
                                                      fec.get_decoder_input_item_size(decoder_obj),
                                                      fec.get_decoder_output_item_size(decoder_obj),
                                                      lentagname, mtu))

        if fec.get_decoder_output_conversion(decoder_obj) == "unpack":
            self.blocks.append(blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST));

        self.connect((self, 0), (self.blocks[0], 0));
        self.connect((self.blocks[-1], 0), (self, 0));

        for i in range(len(self.blocks) - 1):
            self.connect((self.blocks[i], 0), (self.blocks[i+1], 0));
