#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
import numpy

import pmt
from gnuradio import gr, gr_unittest
from gnuradio import digital
from gnuradio import blocks
from gnuradio import channels
from gnuradio.digital.ofdm_txrx import ofdm_tx, ofdm_rx
from gnuradio.digital.utils import tagged_streams

# Set this to true if you need to write out data
LOG_DEBUG_INFO=False

class ofdm_tx_fg (gr.top_block):
    def __init__(self, data, len_tag_key, scramble_bits=False, additional_tags=[]):
        gr.top_block.__init__(self, "ofdm_tx")
        tx_data, tags = tagged_streams.packets_to_vectors((data,), len_tag_key)
        src = blocks.vector_source_b(data, False, 1, tags + additional_tags)
        self.tx = ofdm_tx(packet_length_tag_key=len_tag_key, debug_log=LOG_DEBUG_INFO, scramble_bits=scramble_bits)
        self.sink = blocks.vector_sink_c()
        self.connect(src, self.tx, self.sink)

    def get_tx_samples(self):
        return self.sink.data()

class ofdm_rx_fg (gr.top_block):
    def __init__(self, samples, len_tag_key, channel=None, prepend_zeros=100, scramble_bits=False):
        gr.top_block.__init__(self, "ofdm_rx")
        if prepend_zeros:
            samples = (0,) * prepend_zeros + tuple(samples)
        src = blocks.vector_source_c(tuple(samples) + (0,) * 1000)
        self.rx = ofdm_rx(frame_length_tag_key=len_tag_key, debug_log=LOG_DEBUG_INFO, scramble_bits=scramble_bits)
        if channel is not None:
            self.connect(src, channel, self.rx)
        else:
            self.connect(src, self.rx)
        self.sink = blocks.vector_sink_b()
        self.connect(self.rx, self.sink)

    def get_rx_bytes(self):
        return self.sink.data()

class test_ofdm_txrx (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_tx (self):
        """ Just make sure the Tx works in general """
        # This tag gets put onto the first item of the transmit data,
        # it should be transmitted first, too
        timing_tag = gr.tag_t()
        timing_tag.offset = 0
        timing_tag.key = pmt.string_to_symbol('tx_timing')
        timing_tag.value = pmt.to_pmt('now')
        len_tag_key = 'frame_len'
        n_bytes = 52
        n_samples_expected = (numpy.ceil(1.0 * (n_bytes + 4) / 6) + 3) * 80
        test_data = [random.randint(0, 255) for x in range(n_bytes)]
        tx_fg = ofdm_tx_fg(test_data, len_tag_key, additional_tags=[timing_tag,])
        tx_fg.run()
        self.assertEqual(len(tx_fg.get_tx_samples()), n_samples_expected)
        tags_rx = [gr.tag_to_python(x) for x in tx_fg.sink.tags()]
        tags_rx = sorted([(x.offset, x.key, x.value) for x in tags_rx])
        tags_expected = [
                (0, 'frame_len', n_samples_expected),
                (0, 'tx_timing', 'now'),
        ]
        self.assertEqual(tags_rx, tags_expected)

    def test_002_rx_only_noise(self):
        """ Run the RX with only noise, check it doesn't crash
        or return a burst. """
        len_tag_key = 'frame_len'
        samples = (0,) * 1000
        channel = channels.channel_model(0.1)
        rx_fg = ofdm_rx_fg(samples, len_tag_key, channel)
        rx_fg.run()
        self.assertEqual(len(rx_fg.get_rx_bytes()), 0)

    def test_003_tx1packet(self):
        """ Transmit one packet, with slight AWGN and slight frequency + timing offset.
        Check packet is received and no bit errors have occurred. """
        len_tag_key = 'frame_len'
        n_bytes = 21
        fft_len = 64
        test_data = tuple([random.randint(0, 255) for x in range(n_bytes)])
        # 1.0/fft_len is one sub-carrier, a fine freq offset stays below that
        freq_offset = 1.0 / fft_len * 0.7
        #channel = channels.channel_model(0.01, freq_offset)
        channel = None
        # Tx
        tx_fg = ofdm_tx_fg(test_data, len_tag_key)
        tx_fg.run()
        tx_samples = tx_fg.get_tx_samples()
        # Rx
        rx_fg = ofdm_rx_fg(tx_samples, len_tag_key, channel, prepend_zeros=100)
        rx_fg.run()
        rx_data = rx_fg.get_rx_bytes()
        self.assertEqual(tuple(tx_fg.tx.sync_word1), tuple(rx_fg.rx.sync_word1))
        self.assertEqual(tuple(tx_fg.tx.sync_word2), tuple(rx_fg.rx.sync_word2))
        self.assertEqual(test_data, rx_data)

    def test_003_tx1packet_scramble(self):
        """ Same as before, use scrambler. """
        len_tag_key = 'frame_len'
        n_bytes = 21
        fft_len = 64
        test_data = tuple([random.randint(0, 255) for x in range(n_bytes)])
        # 1.0/fft_len is one sub-carrier, a fine freq offset stays below that
        freq_offset = 1.0 / fft_len * 0.7
        #channel = channels.channel_model(0.01, freq_offset)
        channel = None
        # Tx
        tx_fg = ofdm_tx_fg(test_data, len_tag_key, scramble_bits=True)
        tx_fg.run()
        tx_samples = tx_fg.get_tx_samples()
        # Rx
        rx_fg = ofdm_rx_fg(tx_samples, len_tag_key, channel, prepend_zeros=100, scramble_bits=True)
        rx_fg.run()
        rx_data = rx_fg.get_rx_bytes()
        self.assertEqual(tuple(tx_fg.tx.sync_word1), tuple(rx_fg.rx.sync_word1))
        self.assertEqual(tuple(tx_fg.tx.sync_word2), tuple(rx_fg.rx.sync_word2))
        self.assertEqual(test_data, rx_data)

    def test_004_tx1packet_large_fO(self):
        """ Transmit one packet, with slight AWGN and large frequency offset.
        Check packet is received and no bit errors have occurred. """
        fft_len = 64
        len_tag_key = 'frame_len'
        n_bytes = 21
        test_data = tuple([random.randint(0, 255) for x in range(n_bytes)])
        #test_data = tuple([255 for x in range(n_bytes)])
        # 1.0/fft_len is one sub-carrier
        frequency_offset = 1.0 / fft_len * 2.5
        channel = channels.channel_model(0.00001, frequency_offset)
        # Tx
        tx_fg = ofdm_tx_fg(test_data, len_tag_key)
        tx_fg.run()
        tx_samples = tx_fg.get_tx_samples()
        # Rx
        rx_fg = ofdm_rx_fg(tx_samples, len_tag_key, channel, prepend_zeros=100)
        rx_fg.run()
        rx_data = rx_fg.get_rx_bytes()
        self.assertEqual(test_data, rx_data)

if __name__ == '__main__':
    gr_unittest.run(test_ofdm_txrx, "test_ofdm_txrx.xml")

