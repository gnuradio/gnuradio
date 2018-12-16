#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

from __future__ import division

import random
import numpy

from gnuradio import gr, gr_unittest, blocks, analog, channels
from gnuradio import digital
from gnuradio.digital.utils import tagged_streams
from gnuradio.digital.ofdm_txrx import ofdm_tx

def make_bpsk_burst(fft_len, cp_len, num_bits):
    """ Create a burst of a sync symbol and some BPSK bits """
    sync_symbol = [
        (random.randint(0, 1)*2)-1
        for x in range(fft_len // 2)
    ] * 2
    sync_symbols = sync_symbol[-cp_len:] + sync_symbol
    mod_symbols = [
        (random.randint(0, 1)*2)-1
        for x in range(num_bits)
    ]
    return sync_symbols + mod_symbols

class qa_ofdm_sync_sc_cfb (gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_001_detect(self):
        """ Send two bursts, with zeros in between, and check
        they are both detected at the correct position and no
        false alarms occur """
        n_zeros = 15
        fft_len = 32
        cp_len = 4
        sig_len = (fft_len + cp_len) * 10
        tx_signal = [0,] * n_zeros + make_bpsk_burst(fft_len, cp_len, sig_len)
        tx_signal = tx_signal * 2
        add = blocks.add_cc()
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq = blocks.vector_sink_f()
        sink_detect = blocks.vector_sink_b()
        self.tb.connect(blocks.vector_source_c(tx_signal), (add, 0))
        self.tb.connect(analog.noise_source_c(analog.GR_GAUSSIAN, .01), (add, 1))
        self.tb.connect(add, sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        sig1_detect = sink_detect.data()[0:len(tx_signal) // 2]
        sig2_detect = sink_detect.data()[len(tx_signal) // 2:]
        self.assertTrue(abs(sig1_detect.index(1) - (n_zeros + fft_len + cp_len)) < cp_len)
        self.assertTrue(abs(sig2_detect.index(1) - (n_zeros + fft_len + cp_len)) < cp_len)
        self.assertEqual(numpy.sum(sig1_detect), 1)
        self.assertEqual(numpy.sum(sig2_detect), 1)

    def test_002_freq(self):
        """ Add a fine frequency offset and see if that gets detected properly """
        fft_len = 32
        cp_len = 4
        # This frequency offset is normalized to rads, i.e. \pi == f_s/2
        max_freq_offset = 2*numpy.pi/fft_len # Otherwise, it's coarse
        freq_offset = ((2 * random.random()) - 1) * max_freq_offset
        sig_len = (fft_len + cp_len) * 10
        tx_signal = make_bpsk_burst(fft_len, cp_len, sig_len)
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len, True)
        sink_freq = blocks.vector_sink_f()
        sink_detect = blocks.vector_sink_b()
        channel = channels.channel_model(0.005, freq_offset / 2.0 / numpy.pi)
        self.tb.connect(blocks.vector_source_c(tx_signal), channel, sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        phi_hat = sink_freq.data()[sink_detect.data().index(1)]
        est_freq_offset = 2 * phi_hat / fft_len
        self.assertAlmostEqual(est_freq_offset, freq_offset, places=2)

    def test_003_multiburst(self):
        """ Send several bursts, see if the number of detects is correct.
        Burst lengths and content are random.

        The channel is assumed AWGN for this test.
        """
        n_bursts = 42
        fft_len = 32
        cp_len = 4
        tx_signal = []
        for _ in range(n_bursts):
            gap = [0,] * random.randint(0, 2*fft_len)
            tx_signal += \
                gap + \
                make_bpsk_burst(fft_len, cp_len, fft_len * random.randint(5, 23))
        # Very loose definition of SNR here
        snr = 20 # dB
        sigma = 10**(-snr/10)
        # Add noise -- we don't use the channel model blocks, we want to keep
        # this test as self-contained as possible, and all randomness should
        # derive from random.seed() above
        complex_randn = \
            lambda N: (numpy.random.randn(N) + 1j * numpy.random.randn(N)) * sigma / numpy.sqrt(2)
        tx_signal += complex_randn(len(tx_signal))
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq = blocks.vector_sink_f()
        sink_detect = blocks.vector_sink_b()
        self.tb.connect(blocks.vector_source_c(tx_signal), sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        n_bursts_detected = numpy.sum(sink_detect.data())
        self.assertEqual(
            n_bursts_detected, n_bursts,
            msg="Detection error (missed bursts): {}".format(
                (numpy.sum(sink_detect.data()) - n_bursts))
        )

    def test_004_ofdm_packets(self):
        """
        Send several bursts using ofdm_tx, see if the number of detects is correct.
        Burst lengths and content are random.
        """
        n_bursts = 42
        fft_len = 64
        cp_len = 16
        # Here, coarse freq offset is allowed
        max_freq_offset = 2*numpy.pi/fft_len * 4
        freq_offset = ((2 * random.random()) - 1) * max_freq_offset
        packets = []
        tagname = "packet_length"
        min_packet_length = 10
        max_packet_length = 50
        for _ in range(n_bursts):
            packet_length = random.randint(min_packet_length,
                                           max_packet_length+1)
            packet = [random.randint(0, 255) for i in range(packet_length)]
            packets.append(packet)
        data, tags = tagged_streams.packets_to_vectors(packets, tagname, vlen=1)
        src = blocks.vector_source_b(data, False, 1, tags)
        mod = ofdm_tx(packet_length_tag_key=tagname)
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq = blocks.vector_sink_f()
        sink_detect = blocks.vector_sink_b()
        noise_level = 0.005
        channel = channels.channel_model(noise_level, freq_offset / 2 / numpy.pi)
        self.tb.connect(src, mod, channel, sync, sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        self.assertEqual(numpy.sum(sink_detect.data()), n_bursts)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_sync_sc_cfb, "qa_ofdm_sync_sc_cfb.xml")
