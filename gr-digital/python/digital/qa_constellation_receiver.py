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

import random
import math

import pmt

from gnuradio import gr, gr_unittest, filter, analog, blocks, digital
from gnuradio.digital.utils import mod_codes, alignment
from gnuradio.digital import packet_utils
from gnuradio.digital.generic_mod_demod import generic_mod, generic_demod

from qa_constellation import tested_constellations, twod_constell

# Set a seed so that if errors turn up they are reproducible.
SEED = 1239

# TESTING PARAMETERS
# The number of symbols to test with.
# We need this many to let the frequency recovery block converge.
DATA_LENGTH = 1000
# Test fails if fraction of output that is correct is less than this.
EASY_REQ_CORRECT = 0.9
# For constellations that aren't expected to work so well.
MEDIUM_REQ_CORRECT = 0.8

# CHANNEL PARAMETERS
NOISE_VOLTAGE = 0.01
FREQUENCY_OFFSET = 0.01
TIMING_OFFSET = 1.0

# RECEIVER PARAMETERS
FREQ_BW = 2*math.pi/100.0
PHASE_BW = 2*math.pi/100.0

class channel_model(gr.hier_block2):
    def __init__(self, noise_voltage, freq, timing):
	gr.hier_block2.__init__(self, "channel_model",
				gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))
        

        timing_offset = filter.fractional_resampler_cc(0, timing)
        noise_adder = blocks.add_cc()
        noise = analog.noise_source_c(analog.GR_GAUSSIAN,
                                      noise_voltage, 0)
        freq_offset = analog.sig_source_c(1, analog.GR_SIN_WAVE,
                                          freq, 1.0, 0.0)
        mixer_offset = blocks.multiply_cc();

        self.connect(self, timing_offset)
        self.connect(timing_offset, (mixer_offset,0))
        self.connect(freq_offset, (mixer_offset,1))
        self.connect(mixer_offset, (noise_adder,1))
        self.connect(noise, (noise_adder,0))
        self.connect(noise_adder, self)
        

class test_constellation_receiver(gr_unittest.TestCase):
    
    # We ignore the first half of the output data since often it takes
    # a while for the receiver to lock on.
    ignore_fraction = 0.8
    max_data_length = DATA_LENGTH * 6
    max_num_samples = 1000
    
    def test_basic(self):
        """
        Tests a bunch of different constellations by using generic
        modulation, a channel, and generic demodulation.  The generic
        demodulation uses constellation_receiver which is what
        we're really trying to test.
        """

        rndm = random.Random()
        rndm.seed(SEED)
        # Assumes not more than 64 points in a constellation
        # Generates some random input data to use.
        self.src_data = tuple(
            [rndm.randint(0,1) for i in range(0, self.max_data_length)])
        # Generates some random indices to use for comparing input and
        # output data (a full comparison is too slow in python).
        self.indices = alignment.random_sample(
            self.max_data_length, self.max_num_samples, SEED)

        requirements = (
            (EASY_REQ_CORRECT, tested_constellations(easy=True, medium=False, difficult=False)),
            (MEDIUM_REQ_CORRECT, tested_constellations(easy=False, medium=True, difficult=False)),
            )
        for req_correct, tcs in requirements:
            for constellation, differential in tcs:
                # The constellation_receiver doesn't work for constellations
                # of multiple dimensions (i.e. multiple complex numbers to a
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
                if correct <= req_correct:
                    print("Constellation is {0}. Differential is {1}.  Required correct is {2}. Correct is {3}. FAIL.".
                          format(constellation, differential, req_correct, correct))
                self.assertTrue(correct > req_correct)

    def test_tag(self):
        # Send data through bpsk receiver
        # followed by qpsk receiver
        data = [0.9+0j, 0.1+0.9j, -1-0.1j, -0.1-0.6j]*2
        bpsk_data = [1, 1, 0, 0]
        qpsk_data = [1, 3, 0, 0]
        first_tag = gr.tag_t()
        first_tag.key = pmt.intern("set_constellation")
        first_tag.value = digital.bpsk_constellation().as_pmt()
        first_tag.offset = 0
        second_tag = gr.tag_t()
        second_tag.key = pmt.intern("set_constellation")
        second_tag.value = digital.qpsk_constellation().as_pmt()
        second_tag.offset = 4
        src = blocks.vector_source_c(data, False, 1, [first_tag, second_tag])
        decoder = digital.constellation_receiver_cb(
            digital.bpsk_constellation().base(), 0, 0, 0)
        snk = blocks.vector_sink_b()
        tb = gr.top_block()
        tb.connect(src, decoder, snk)
        tb.run()
        self.assertEqual(list(snk.data()), bpsk_data+qpsk_data)

class rec_test_tb(gr.top_block):
    """
    Takes a constellation an runs a generic modulation, channel,
    and generic demodulation.
    """
    def __init__(self, constellation, differential,
                 data_length=None, src_data=None, freq_offset=True):
        """
        Args:
            constellation: a constellation object
            differential: whether differential encoding is used
            data_length: the number of bits of data to use
            src_data: a list of the bits to use
            freq_offset: whether to use a frequency offset in the channel
        """
        super(rec_test_tb, self).__init__()
        # Transmission Blocks
        if src_data is None:
            self.src_data = tuple([rndm.randint(0,1) for i in range(0, data_length)])
        else:
            self.src_data = src_data
        packer = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        src = blocks.vector_source_b(self.src_data)
        mod = generic_mod(constellation, differential=differential)
        # Channel
        if freq_offset:
            channel = channel_model(NOISE_VOLTAGE, FREQUENCY_OFFSET, TIMING_OFFSET)
        else:
            channel = channel_model(NOISE_VOLTAGE, 0, TIMING_OFFSET)            
        # Receiver Blocks
        if freq_offset:
            demod = generic_demod(constellation, differential=differential,
                                  freq_bw=FREQ_BW,
                                  phase_bw=PHASE_BW)
        else:
            demod = generic_demod(constellation, differential=differential,
                                  freq_bw=0, phase_bw=0)
        self.dst = blocks.vector_sink_b()
        self.connect(src, packer, mod, channel, demod, self.dst)

if __name__ == '__main__':
    gr_unittest.run(test_constellation_receiver, "test_constellation_receiver.xml")
