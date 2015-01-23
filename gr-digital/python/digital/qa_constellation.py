#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
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

import random, math
from cmath import exp, pi, log, sqrt

from gnuradio import gr, gr_unittest, digital, blocks
from gnuradio.digital.utils import mod_codes
from gnuradio.digital import psk, qam, qamlike

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
    return digital.constellation_calcdist(points, [], rot_sym, dim)

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
    return digital.constellation_calcdist(points, [], rot_sym, dim)

# A list of tuples for constellation testing.  The contents of the
# tuples are (constructor, poss_args, differential, diff_argname).

# These constellations should lock on well.
easy_constellation_info = (
    (psk.psk_constellation,
     {'m': (2, 4, 8, 16, ),
      'mod_code': tested_mod_codes, },
     True, None),
    (psk.psk_constellation,
     {'m': (2, 4, 8, 16, 32, 64),
      'mod_code': tested_mod_codes,
      'differential': (False,)},
     False, None),
    (qam.qam_constellation,
     {'constellation_points': (4,),
      'mod_code': tested_mod_codes,
      'large_ampls_to_corners': [False],},
     True, None),
    (qam.qam_constellation,
     {'constellation_points': (4, 16, 64),
      'mod_code': tested_mod_codes,
      'differential': (False,)},
     False, None),
    (digital.constellation_bpsk, {}, True, None),
    (digital.constellation_qpsk, {}, False, None),
    (digital.constellation_dqpsk, {}, True, None),
    (digital.constellation_8psk, {}, False, None),
    (twod_constell, {}, True, None),
    (threed_constell, {}, True, None),
    )

# These constellations don't work nicely.
# We have a lower required error rate.
medium_constellation_info = (
    (psk.psk_constellation,
     {'m': (32, 64),
      'mod_code': tested_mod_codes, },
     True, None),
    (qam.qam_constellation,
     {'constellation_points': (16 ,),
      'mod_code': tested_mod_codes,
      'large_ampls_to_corners': [False, True],},
     True, None),
    (qamlike.qam32_holeinside_constellation,
     {'large_ampls_to_corners': [True]},
     True, None),
)

# These constellation are basically broken in our test
difficult_constellation_info = (
    (qam.qam_constellation,
     {'constellation_points': (64,),
      'mod_code': tested_mod_codes,
      'large_ampls_to_corners': [False, True],},
     True, None),
)

def slicer(x):
    ret = []
    for xi in x:
        if(xi < 0):
            ret.append(0.0)
    else:
        ret.append(1.0)
    return ret

def tested_constellations(easy=True, medium=True, difficult=True):
    """
    Generator to produce (constellation, differential) tuples for testing purposes.
    """
    constellation_info = []
    if easy:
        constellation_info += easy_constellation_info
    if medium:
        constellation_info += medium_constellation_info
    if difficult:
        constellation_info += difficult_constellation_info
    for constructor, poss_args, differential, diff_argname in constellation_info:
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


class test_constellation(gr_unittest.TestCase):

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
                src = blocks.vector_source_b(self.src_data)
                content = mod_demod(constellation, differential, rotation)
                dst = blocks.vector_sink_b()
                self.tb = gr.top_block()
                self.tb.connect(src, content, dst)
                self.tb.run()
                data = dst.data()
                # Don't worry about cut off data for now.
                first = constellation.bits_per_symbol()
                self.assertEqual(self.src_data[first:len(data)], data[first:])

    def test_soft_qpsk_gen(self):
        prec = 8
        constel, code = digital.psk_4_0()

        rot_sym = 1
        side = 2
        width = 2
        c = digital.constellation_rect(constel, code, rot_sym,
                                       side, side, width, width)

        # Get max energy/symbol in constellation
        constel = c.points()
        Es = max([abs(constel_i) for constel_i in constel])

        table = digital.soft_dec_table_generator(digital.sd_psk_4_0, prec, Es)
        c.set_soft_dec_lut(table, prec)

        x = sqrt(2.0)/2.0
        step = (x.real+x.real) / (2**prec - 1)
        samples = [ -x-x*1j, -x+x*1j,
                     x+x*1j,  x-x*1j,
                   (-x+128*step)+(-x+128*step)*1j,
                   (-x+64*step) +(-x+64*step)*1j,  (-x+64*step) +(-x+192*step)*1j,
                   (-x+192*step)+(-x+192*step)*1j, (-x+192*step)+(-x+64*step)*1j,]

        y_python_raw_calc = []
        y_python_gen_calc = []
        y_python_table = []
        y_cpp_raw_calc = []
        y_cpp_table = []
        for sample in samples:
            y_python_raw_calc += slicer(digital.calc_soft_dec(sample, constel, code))
            y_python_gen_calc += slicer(digital.sd_psk_4_0(sample, Es))
            y_python_table += slicer(digital.calc_soft_dec_from_table(sample, table, prec, Es))

            y_cpp_raw_calc += c.calc_soft_dec(sample)
            y_cpp_table += c.soft_decision_maker(sample)

        self.assertFloatTuplesAlmostEqual(y_python_raw_calc, y_python_gen_calc, 0)
        self.assertFloatTuplesAlmostEqual(y_python_gen_calc, y_python_table, 0)
        self.assertFloatTuplesAlmostEqual(y_cpp_raw_calc, y_cpp_table, 0)

    def test_soft_qpsk_calc(self):
        prec = 8
        constel, code = digital.psk_4_0()

        rot_sym = 1
        side = 2
        width = 2
        c = digital.constellation_rect(constel, code, rot_sym,
                                       side, side, width, width)

        # Get max energy/symbol in constellation
        constel = c.points()
        Es = max([abs(constel_i) for constel_i in constel])

        table = digital.soft_dec_table(constel, code, prec)
        c.gen_soft_dec_lut(prec)

        x = sqrt(2.0)/2.0
        step = (x.real+x.real) / (2**prec - 1)
        samples = [ -x-x*1j, -x+x*1j,
                     x+x*1j,  x-x*1j,
                   (-x+128*step)+(-x+128*step)*1j,
                   (-x+64*step) +(-x+64*step)*1j,  (-x+64*step) +(-x+192*step)*1j,
                   (-x+192*step)+(-x+192*step)*1j, (-x+192*step)+(-x+64*step)*1j,]

        y_python_raw_calc = []
        y_python_table = []
        y_cpp_raw_calc = []
        y_cpp_table = []
        for sample in samples:
            y_python_raw_calc += slicer(digital.calc_soft_dec(sample, constel, code))
            y_python_table += slicer(digital.calc_soft_dec_from_table(sample, table, prec, Es))

            y_cpp_raw_calc += slicer(c.calc_soft_dec(sample))
            y_cpp_table += slicer(c.soft_decision_maker(sample))

        self.assertEqual(y_python_raw_calc, y_python_table)
        self.assertEqual(y_cpp_raw_calc, y_cpp_table)


    def test_soft_qam16_calc(self):
        prec = 8
        constel, code = digital.qam_16_0()

        rot_sym = 1
        side = 2
        width = 2
        c = digital.constellation_rect(constel, code, rot_sym,
                                       side, side, width, width)

        # Get max energy/symbol in constellation
        constel = c.points()
        Es = max([abs(constel_i) for constel_i in constel])

        table = digital.soft_dec_table(constel, code, prec)
        c.gen_soft_dec_lut(prec)

        x = sqrt(2.0)/2.0
        step = (x.real+x.real) / (2**prec - 1)
        samples = [ -x-x*1j, -x+x*1j,
                     x+x*1j,  x-x*1j,
                   (-x+128*step)+(-x+128*step)*1j,
                   (-x+64*step) +(-x+64*step)*1j,  (-x+64*step) +(-x+192*step)*1j,
                   (-x+192*step)+(-x+192*step)*1j, (-x+192*step)+(-x+64*step)*1j,]

        y_python_raw_calc = []
        y_python_table = []
        y_cpp_raw_calc = []
        y_cpp_table = []
        for sample in samples:
            y_python_raw_calc += slicer(digital.calc_soft_dec(sample, constel, code))
            y_python_table += slicer(digital.calc_soft_dec_from_table(sample, table, prec, Es))

            y_cpp_raw_calc += slicer(c.calc_soft_dec(sample))
            y_cpp_table += slicer(c.soft_decision_maker(sample))

        self.assertFloatTuplesAlmostEqual(y_python_raw_calc, y_python_table, 0)
        self.assertFloatTuplesAlmostEqual(y_cpp_raw_calc, y_cpp_table, 0)

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
        import weakref
        self.blocks = [weakref.proxy(self)]
        # We expect a stream of unpacked bits.
        # First step is to pack them.
        self.blocks.append(blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST))
        # Second step we unpack them such that we have k bits in each byte where
        # each constellation symbol hold k bits.
        self.blocks.append(
            blocks.packed_to_unpacked_bb(self.constellation.bits_per_symbol(),
                                         gr.GR_MSB_FIRST))
        # Apply any pre-differential coding
        # Gray-coding is done here if we're also using differential coding.
        if self.constellation.apply_pre_diff_code():
            self.blocks.append(digital.map_bb(self.constellation.pre_diff_code()))
        # Differential encoding.
        if self.differential:
            self.blocks.append(digital.diff_encoder_bb(arity))
        # Convert to constellation symbols.
        self.blocks.append(digital.chunks_to_symbols_bc(self.constellation.points(),
                                                        self.constellation.dimensionality()))
        # CHANNEL
        # Channel just consists of a rotation to check differential coding.
        if rotation is not None:
            self.blocks.append(blocks.multiply_const_cc(rotation))

        # RX
        # Convert the constellation symbols back to binary values.
        self.blocks.append(digital.constellation_decoder_cb(self.constellation.base()))
        # Differential decoding.
        if self.differential:
            self.blocks.append(digital.diff_decoder_bb(arity))
        # Decode any pre-differential coding.
        if self.constellation.apply_pre_diff_code():
            self.blocks.append(digital.map_bb(
                mod_codes.invert_code(self.constellation.pre_diff_code())))
        # unpack the k bit vector into a stream of bits
        self.blocks.append(blocks.unpack_k_bits_bb(
                self.constellation.bits_per_symbol()))
        # connect to block output
        check_index = len(self.blocks)
        self.blocks = self.blocks[:check_index]
        self.blocks.append(weakref.proxy(self))

        self.connect(*self.blocks)

if __name__ == '__main__':
    gr_unittest.run(test_constellation, "test_constellation.xml")
