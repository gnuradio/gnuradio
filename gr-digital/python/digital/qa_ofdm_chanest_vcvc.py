#!/usr/bin/env python
# Copyright 2012-2014 Free Software Foundation, Inc.
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

import sys
import numpy
import random
import numpy

from gnuradio import gr, gr_unittest, blocks, analog, digital
import pmt

def shift_tuple(vec, N):
    """ Shifts a vector by N elements. Fills up with zeros. """
    if N > 0:
        return (0,) * N + tuple(vec[0:-N])
    else:
        N = -N
        return tuple(vec[N:]) + (0,) * N

def rand_range(min_val, max_val):
    """ Returns a random value (uniform) from the interval min_val, max_val """
    return random.random() * (max_val - min_val) + min_val


class qa_ofdm_chanest_vcvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_offset_2sym (self):
        """ Add a frequency offset, check if it's correctly detected.
        Also add some random tags and see if they come out at the correct
        position. """
        fft_len = 16
        carr_offset = -2
        sync_symbol1 = (0, 0, 0, 1,  0, 1,  0, -1, 0, 1,  0, -1,  0, 1, 0, 0)
        sync_symbol2 = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        data_symbol  = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        tx_data = shift_tuple(sync_symbol1, carr_offset) + \
                  shift_tuple(sync_symbol2, carr_offset) + \
                  shift_tuple(data_symbol, carr_offset)
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol("test_tag_1")
        tag1.value = pmt.from_long(23)
        tag2 = gr.tag_t()
        tag2.offset = 2
        tag2.key = pmt.string_to_symbol("test_tag_2")
        tag2.value = pmt.from_long(42)
        src = blocks.vector_source_c(tx_data, False, fft_len, (tag1, tag2))
        chanest = digital.ofdm_chanest_vcvc(sync_symbol1, sync_symbol2, 1)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chanest, sink)
        self.tb.run()
        self.assertEqual(shift_tuple(sink.data(), -carr_offset), data_symbol)
        tags = sink.tags()
        ptags = {}
        for tag in tags:
            ptag = gr.tag_to_python(tag)
            ptags[ptag.key] = (ptag.value, ptag.offset)
            if ptag.key == 'ofdm_sync_chan_taps':
                ptags[ptag.key] = (None, ptag.offset)
        expected_tags = {
                'ofdm_sync_carr_offset': (-2, 0),
                'ofdm_sync_chan_taps': (None, 0),
                'test_tag_1': (23, 0),
                'test_tag_2': (42, 0),
        }
        self.assertEqual(ptags, expected_tags)


    def test_002_offset_1sym (self):
        """ Add a frequency offset, check if it's correctly detected.
        Difference to previous test is, it only uses one synchronisation symbol. """
        fft_len = 16
        carr_offset = -2
        # This will not correct for +2 because it thinks carrier 14 is used
        # (because of interpolation)
        sync_symbol = (0, 0, 0, 1,  0, 1,  0, -1, 0, 1,  0, -1,  0, 1, 0, 0)
        data_symbol = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        tx_data = shift_tuple(sync_symbol, carr_offset) + \
                  shift_tuple(data_symbol, carr_offset)
        src = blocks.vector_source_c(tx_data, False, fft_len)
        # 17 is out of bounds!
        chanest = digital.ofdm_chanest_vcvc(sync_symbol, (), 1, 0, 17)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chanest, sink)
        self.tb.run()
        self.assertEqual(shift_tuple(sink.data(), -carr_offset), data_symbol)
        tags = sink.tags()
        for tag in tags:
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                carr_offset_hat = pmt.to_long(tag.value)
                self.assertEqual(pmt.to_long(tag.value), carr_offset)

    def test_003_channel_no_carroffset (self):
        """ Add a channel, check if it's correctly estimated """
        fft_len = 16
        carr_offset = 0
        sync_symbol1 = (0, 0, 0, 1,  0, 1,  0, -1, 0, 1,  0, -1,  0, 1, 0, 0)
        sync_symbol2 = (0, 0, 0, 1j, -1, 1, -1j,  1j, 0, 1, -1j, -1, -1j, 1, 0, 0)
        data_symbol  = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        tx_data = sync_symbol1 + sync_symbol2 + data_symbol
        channel = (0, 0, 0, 2, -2, 2, 3j, 2, 0, 2, 2, 2, 2, 3, 0, 0)
        src = blocks.vector_source_c(tx_data, False, fft_len)
        chan = blocks.multiply_const_vcc(channel)
        chanest = digital.ofdm_chanest_vcvc(sync_symbol1, sync_symbol2, 1)
        sink = blocks.vector_sink_c(fft_len)
        sink_chanest = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chan, chanest, sink)
        self.tb.connect((chanest, 1), sink_chanest)
        self.tb.run()
        tags = sink.tags()
        self.assertEqual(shift_tuple(sink.data(), -carr_offset), tuple(numpy.multiply(data_symbol, channel)))
        for tag in tags:
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                self.assertEqual(pmt.to_long(tag.value), carr_offset)
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_chan_taps':
                self.assertEqual(pmt.c32vector_elements(tag.value), channel)
        self.assertEqual(sink_chanest.data(), channel)

    def test_004_channel_no_carroffset_1sym (self):
        """ Add a channel, check if it's correctly estimated.
        Only uses 1 synchronisation symbol. """
        fft_len = 16
        carr_offset = 0
        sync_symbol = (0, 0, 0, 1,  0, 1,  0, -1, 0, 1,  0, -1,  0, 1, 0, 0)
        data_symbol  = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        tx_data = sync_symbol + data_symbol
        channel = (0, 0, 0, 2, 2, 2, 2, 3, 3, 2.5, 2.5, -3, -3, 1j, 1j, 0)
        #channel = (0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
        src = blocks.vector_source_c(tx_data, False, fft_len)
        chan = blocks.multiply_const_vcc(channel)
        chanest = digital.ofdm_chanest_vcvc(sync_symbol, (), 1)
        sink = blocks.vector_sink_c(fft_len)
        sink_chanest = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chan, chanest, sink)
        self.tb.connect((chanest, 1), sink_chanest)
        self.tb.run()
        self.assertEqual(sink_chanest.data(), channel)
        tags = sink.tags()
        for tag in tags:
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                self.assertEqual(pmt.to_long(tag.value), carr_offset)
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_chan_taps':
                self.assertEqual(pmt.c32vector_elements(tag.value), channel)

    def test_005_both_1sym_force (self):
        """ Add a channel, check if it's correctly estimated.
        Only uses 1 synchronisation symbol. """
        fft_len = 16
        carr_offset = 0
        sync_symbol = (0, 0, 0, 1,  0, 1,  0, -1, 0, 1,  0, -1,  0, 1, 0, 0)
        ref_symbol  = (0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0)
        data_symbol = (0, 0, 0, 1, -1, 1, -1,  1, 0, 1, -1, -1, -1, 1, 0, 0)
        tx_data = sync_symbol + data_symbol
        channel = (0, 0, 0, 2, 2, 2, 2.5, 3, 2.5, 2, 2.5, 3, 2, 1, 1, 0)
        src = blocks.vector_source_c(tx_data, False, fft_len)
        chan = blocks.multiply_const_vcc(channel)
        chanest = digital.ofdm_chanest_vcvc(sync_symbol, ref_symbol, 1)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chan, chanest, sink)
        self.tb.run()
        tags = sink.tags()
        for tag in tags:
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                self.assertEqual(pmt.to_long(tag.value), carr_offset)
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_chan_taps':
                self.assertEqual(pmt.c32vector_elements(tag.value), channel)

    def test_006_channel_and_carroffset (self):
        """ Add a channel, check if it's correctly estimated """
        fft_len = 16
        carr_offset = 2
        # Index         0  1  2   3   4  5    6   7  8  9   10  11   12 13 14 15
        sync_symbol1 = (0, 0, 0,  1,  0, 1,  0,  -1, 0, 1,   0, -1,   0, 1, 0, 0)
        sync_symbol2 = (0, 0, 0, 1j, -1, 1, -1j, 1j, 0, 1, -1j, -1, -1j, 1, 0, 0)
        data_symbol  = (0, 0, 0,  1, -1, 1,  -1,  1, 0, 1,  -1, -1,  -1, 1, 0, 0)
        # Channel       0  1  2  3  4   5   6  7    8   9 10 11   12  13   14  15
        # Shifted      (0, 0, 0, 0, 0, 1j, -1, 1, -1j, 1j, 0, 1, -1j, -1, -1j, 1)
        chanest_exp  = (0, 0, 0, 5, 6, 7, 8, 9, 0, 11, 12, 13, 14, 15, 0, 0)
        tx_data = shift_tuple(sync_symbol1, carr_offset) + \
                  shift_tuple(sync_symbol2, carr_offset) + \
                  shift_tuple(data_symbol, carr_offset)
        channel = range(fft_len)
        src = blocks.vector_source_c(tx_data, False, fft_len)
        chan = blocks.multiply_const_vcc(channel)
        chanest = digital.ofdm_chanest_vcvc(sync_symbol1, sync_symbol2, 1)
        sink = blocks.vector_sink_c(fft_len)
        self.tb.connect(src, chan, chanest, sink)
        self.tb.run()
        tags = sink.tags()
        chan_est = None
        for tag in tags:
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                self.assertEqual(pmt.to_long(tag.value), carr_offset)
            if pmt.symbol_to_string(tag.key) == 'ofdm_sync_chan_taps':
                chan_est = pmt.c32vector_elements(tag.value)
        self.assertEqual(chan_est, chanest_exp)
        self.assertEqual(sink.data(), tuple(numpy.multiply(shift_tuple(data_symbol, carr_offset), channel)))


    def test_999_all_at_once(self):
        """docstring for test_999_all_at_once"""
        fft_len = 32
        # 6 carriers empty, 10 carriers full, 1 DC carrier, 10 carriers full, 5 carriers empty
        syncsym_mask = (0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0)
        carrier_mask = (0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0)
        max_offset = 4
        wgn_amplitude = 0.05
        min_chan_ampl = 0.1
        max_chan_ampl = 5
        n_iter = 20 # The more the accurater
        def run_flow_graph(sync_sym1, sync_sym2, data_sym):
            top_block = gr.top_block()
            carr_offset = random.randint(-max_offset/2, max_offset/2) * 2
            tx_data = shift_tuple(sync_sym1, carr_offset) + \
                      shift_tuple(sync_sym2, carr_offset) + \
                      shift_tuple(data_sym,  carr_offset)
            channel = [rand_range(min_chan_ampl, max_chan_ampl) * numpy.exp(1j * rand_range(0, 2 * numpy.pi)) for x in range(fft_len)]
            src = blocks.vector_source_c(tx_data, False, fft_len)
            chan = blocks.multiply_const_vcc(channel)
            noise = analog.noise_source_c(analog.GR_GAUSSIAN, wgn_amplitude)
            add = blocks.add_cc(fft_len)
            chanest = digital.ofdm_chanest_vcvc(sync_sym1, sync_sym2, 1)
            sink = blocks.vector_sink_c(fft_len)
            top_block.connect(src, chan, (add, 0), chanest, sink)
            top_block.connect(noise, blocks.stream_to_vector(gr.sizeof_gr_complex, fft_len), (add, 1))
            top_block.run()
            channel_est = None
            carr_offset_hat = 0
            rx_sym_est = [0,] * fft_len
            tags = sink.tags()
            for tag in tags:
                if pmt.symbol_to_string(tag.key) == 'ofdm_sync_carr_offset':
                    carr_offset_hat = pmt.to_long(tag.value)
                    self.assertEqual(carr_offset, carr_offset_hat)
                if pmt.symbol_to_string(tag.key) == 'ofdm_sync_chan_taps':
                    channel_est = shift_tuple(pmt.c32vector_elements(tag.value), carr_offset)
            shifted_carrier_mask = shift_tuple(carrier_mask, carr_offset)
            for i in range(fft_len):
                if shifted_carrier_mask[i] and channel_est[i]:
                    self.assertAlmostEqual(channel[i], channel_est[i], places=0)
                    rx_sym_est[i] = (sink.data()[i] / channel_est[i]).real
            return (carr_offset, list(shift_tuple(rx_sym_est, -carr_offset_hat)))
        bit_errors = 0
        for k in xrange(n_iter):
            sync_sym = [(random.randint(0, 1) * 2 - 1) * syncsym_mask[i] for i in range(fft_len)]
            ref_sym  = [(random.randint(0, 1) * 2 - 1) * carrier_mask[i] for i in range(fft_len)]
            data_sym = [(random.randint(0, 1) * 2 - 1) * carrier_mask[i] for i in range(fft_len)]
            data_sym[26] = 1
            (carr_offset, rx_sym) = run_flow_graph(sync_sym, ref_sym, data_sym)
            rx_sym_est = [0,] * fft_len
            for i in xrange(fft_len):
                if carrier_mask[i] == 0:
                    continue
                rx_sym_est[i] = {True: 1, False: -1}[rx_sym[i] > 0]
                if rx_sym_est[i] != data_sym[i]:
                    bit_errors += 1
        # This is much more than we could allow
        self.assertTrue(bit_errors < n_iter)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_chanest_vcvc, "qa_ofdm_chanest_vcvc.xml")

