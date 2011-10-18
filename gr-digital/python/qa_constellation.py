#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

import random
from cmath import exp, pi, log

from gnuradio import gr, gr_unittest, blks2
from utils import mod_codes
import digital_swig

# import from local folder
import psk
import qam

tested_mod_codes = (mod_codes.NO_CODE, mod_codes.GRAY_CODE)

# A list of the constellations to test.
# Each constellation is given by a 3-tuple.
# First item is a function to generate the constellation
# Second item is a dictionary of arguments for function with lists of
# possible values.
# Third item is whether differential encoding should be tested.
# Fourth item is the name of the argument to constructor that specifices
# whether differential encoding is used.

def twod_constell():
    """
    
    """
    points = ((1+0j), (0+1j),
              (-1+0j), (0-1j))
    rot_sym = 2
    dim = 2
    return digital_swig.constellation_calcdist(points, [], rot_sym, dim)

def threed_constell():
    oned_points = ((1+0j), (0+1j), (-1+0j), (0-1j))
    points = []
    r4 = range(0, 4)
    for ia in r4:
        for ib in r4:
            for ic in r4:
                points += [oned_points[ia], oned_points[ib], oned_points[ic]]
    rot_sym = 4
    dim = 3
    return digital_swig.constellation_calcdist(points, [], rot_sym, dim)

tested_constellation_info = (
    (psk.psk_constellation,
     {'m': (2, 4, 8, 16, 32, 64),
      'mod_code': tested_mod_codes, },
     True, None),
    (digital_swig.constellation_bpsk, {}, True, None),
    (digital_swig.constellation_qpsk, {}, False, None),
    (digital_swig.constellation_dqpsk, {}, True, None),
    (digital_swig.constellation_8psk, {}, False, None),
    (twod_constell, {}, True, None),
    (threed_constell, {}, True, None),
    )

def tested_constellations():
    """
    Generator to produce (constellation, differential) tuples for testing purposes.
    """
    for constructor, poss_args, differential, diff_argname in tested_constellation_info:
        if differential:
            diff_poss = (True, False)
        else:
            diff_poss = (False,)
        poss_args = [[argname, argvalues, 0] for argname, argvalues in poss_args.items()]
        for current_diff in diff_poss:
            # Add an index into args to keep track of current position in argvalues
            while True:
                current_args = dict([(argname, argvalues[argindex])
                                     for argname, argvalues, argindex in poss_args])
                if diff_argname is not None:
                    current_args[diff_argname] = current_diff
                constellation = constructor(**current_args)
                yield (constellation, current_diff)
                for this_poss_arg in poss_args:
                    argname, argvalues, argindex = this_poss_arg
                    if argindex < len(argvalues) - 1:
                        this_poss_arg[2] += 1
                        break
                    else:
                        this_poss_arg[2] = 0
                if sum([argindex for argname, argvalues, argindex in poss_args]) == 0:
                    break
            

class test_constellation (gr_unittest.TestCase):

    src_length = 256

    def setUp(self):
        # Generate a list of random bits.
        self.src_data = tuple([random.randint(0,1) for i in range(0, self.src_length)])

    def tearDown(self):
        pass

    def test_hard_decision(self):
        for constellation, differential in tested_constellations():
            if differential:
                rs = constellation.rotational_symmetry()
                rotations = [exp(i*2*pi*(0+1j)/rs) for i in range(0, rs)] 
            else:
                rotations = [None]
            for rotation in rotations:
                src = gr.vector_source_b(self.src_data)
                content = mod_demod(constellation, differential, rotation)
                dst = gr.vector_sink_b()
                self.tb = gr.top_block()
                self.tb.connect(src, content, dst)
                self.tb.run()
                data = dst.data()
                # Don't worry about cut off data for now.
                first = constellation.bits_per_symbol()
                self.assertEqual (self.src_data[first:len(data)], data[first:])


class mod_demod(gr.hier_block2):
    def __init__(self, constellation, differential, rotation):
        if constellation.arity() > 256:
            # If this becomes limiting some of the blocks should be generalised so
            # that they can work with shorts and ints as well as chars.
            raise ValueError("Constellation cannot contain more than 256 points.")

	gr.hier_block2.__init__(self, "mod_demod",
				gr.io_signature(1, 1, gr.sizeof_char),       # Input signature
				gr.io_signature(1, 1, gr.sizeof_char))       # Output signature

        arity = constellation.arity()

        # TX
        self.constellation = constellation
        self.differential = differential
        self.blocks = [self]
        # We expect a stream of unpacked bits.
        # First step is to pack them.
        self.blocks.append(
            gr.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST))
        # Second step we unpack them such that we have k bits in each byte where
        # each constellation symbol hold k bits.
        self.blocks.append(
            gr.packed_to_unpacked_bb(self.constellation.bits_per_symbol(),
                                     gr.GR_MSB_FIRST))
        # Apply any pre-differential coding
        # Gray-coding is done here if we're also using differential coding.
        if self.constellation.apply_pre_diff_code():
            self.blocks.append(gr.map_bb(self.constellation.pre_diff_code()))
        # Differential encoding.
        if self.differential:
            self.blocks.append(gr.diff_encoder_bb(arity))
        # Convert to constellation symbols.
        self.blocks.append(gr.chunks_to_symbols_bc(self.constellation.points(),
                                                   self.constellation.dimensionality()))
        # CHANNEL
        # Channel just consists of a rotation to check differential coding.
        if rotation is not None:
            self.blocks.append(gr.multiply_const_cc(rotation))

        # RX
        # Convert the constellation symbols back to binary values.
        self.blocks.append(digital_swig.constellation_decoder_cb(self.constellation.base()))
        # Differential decoding.
        if self.differential:
            self.blocks.append(gr.diff_decoder_bb(arity))
        # Decode any pre-differential coding.
        if self.constellation.apply_pre_diff_code():
            self.blocks.append(gr.map_bb(
                mod_codes.invert_code(self.constellation.pre_diff_code())))
        # unpack the k bit vector into a stream of bits            
        self.blocks.append(gr.unpack_k_bits_bb(
                self.constellation.bits_per_symbol()))
        # connect to block output
        check_index = len(self.blocks)
        self.blocks = self.blocks[:check_index]
        self.blocks.append(self)

        self.connect(*self.blocks)

        
if __name__ == '__main__':
    gr_unittest.run(test_constellation, "test_constellation.xml")
