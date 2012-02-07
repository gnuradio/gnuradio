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

from gnuradio import gr, blks2, gr_unittest
from utils import mod_codes, alignment
import digital_swig, packet_utils
from generic_mod_demod import generic_mod, generic_demod

from qa_constellation import tested_constellations, twod_constell

import math

# Set a seed so that if errors turn up they are reproducible.
# 1234 fails
random.seed(1239)

# TESTING PARAMETERS
# The number of symbols to test with.
# We need this many to let the frequency recovery block converge.
DATA_LENGTH = 2000
# Test fails if fraction of output that is correct is less than this.
REQ_CORRECT = 0.8

# CHANNEL PARAMETERS
NOISE_VOLTAGE = 0.01
FREQUENCY_OFFSET = 0.01
TIMING_OFFSET = 1.0

# RECEIVER PARAMETERS
FREQ_BW = 2*math.pi/100.0
PHASE_BW = 2*math.pi/100.0


class test_constellation_receiver (gr_unittest.TestCase):
    
    # We ignore the first half of the output data since often it takes
    # a while for the receiver to lock on.
    ignore_fraction = 0.8
    seed = 1234
    max_data_length = DATA_LENGTH * 6
    max_num_samples = 1000
    
    def test_basic(self):
        """
        Tests a bunch of different constellations by using generic
        modulation, a channel, and generic demodulation.  The generic
        demodulation uses constellation_receiver which is what
        we're really trying to test.
        """

        # Assumes not more than 64 points in a constellation
        # Generates some random input data to use.
        self.src_data = tuple(
            [random.randint(0,1) for i in range(0, self.max_data_length)])
        # Generates some random indices to use for comparing input and
        # output data (a full comparison is too slow in python).
        self.indices = alignment.random_sample(
            self.max_data_length, self.max_num_samples, self.seed)

        for constellation, differential in tested_constellations():
            # The constellation_receiver doesn't work for constellations
            # of multple dimensions (i.e. multiple complex numbers to a
            # single symbol).
            # That is not implemented since the receiver has no way of
            # knowing where the beginning of a symbol is.
            # It also doesn't work for non-differential modulation.
            if constellation.dimensionality() != 1 or not differential:
                continue
            data_length = DATA_LENGTH * constellation.bits_per_symbol()
            tb = rec_test_tb(constellation, differential,
                             src_data=self.src_data[:data_length])
            tb.run()
            data = tb.dst.data()
            d1 = tb.src_data[:int(len(tb.src_data)*self.ignore_fraction)]
            d2 = data[:int(len(data)*self.ignore_fraction)]
            correct, overlap, offset, indices = alignment.align_sequences(
                d1, d2, indices=self.indices)
            self.assertTrue(correct > REQ_CORRECT)
            

class rec_test_tb (gr.top_block):
    """
    Takes a constellation an runs a generic modulation, channel,
    and generic demodulation.
    """
    def __init__(self, constellation, differential,
                 data_length=None, src_data=None):
        """
        constellation -- a constellation object
        differential -- whether differential encoding is used
        data_length -- the number of bits of data to use
        src_data -- a list of the bits to use
        """
        super(rec_test_tb, self).__init__()
        # Transmission Blocks
        if src_data is None:
            self.src_data = tuple([random.randint(0,1) for i in range(0, data_length)])
        else:
            self.src_data = src_data
        packer = gr.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        src = gr.vector_source_b(self.src_data)
        mod = generic_mod(constellation, differential=differential)
        # Channel
        channel = gr.channel_model(NOISE_VOLTAGE, FREQUENCY_OFFSET, TIMING_OFFSET)
        # Receiver Blocks
        demod = generic_demod(constellation, differential=differential,
                              freq_bw=FREQ_BW,
                              phase_bw=PHASE_BW)
        self.dst = gr.vector_sink_b()
        self.connect(src, packer, mod, channel, demod, self.dst)

if __name__ == '__main__':
    gr_unittest.run(test_constellation_receiver, "test_constellation_receiver.xml")
