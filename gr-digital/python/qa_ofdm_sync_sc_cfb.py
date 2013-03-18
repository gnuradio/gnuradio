#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

import numpy
import random

from gnuradio import gr, gr_unittest
import blocks_swig as blocks
import analog_swig as analog

try:
    # This will work when feature #505 is added.
    from gnuradio import digital
    from gnuradio.digital.utils import tagged_streams
    from gnuradio.digital.ofdm_txrx import ofdm_tx
except ImportError:
    # Until then this will work.
    import digital_swig as digital
    from utils import tagged_streams
    from ofdm_txrx import ofdm_tx


class qa_ofdm_sync_sc_cfb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_detect (self):
        """ Send two bursts, with zeros in between, and check
        they are both detected at the correct position and no
        false alarms occur """
        n_zeros = 15
        fft_len = 32
        cp_len = 4
        sig_len = (fft_len + cp_len) * 10
        sync_symbol = [(random.randint(0, 1)*2)-1 for x in range(fft_len/2)] * 2
        tx_signal = [0,] * n_zeros + \
                    sync_symbol[-cp_len:] + \
                    sync_symbol + \
                    [(random.randint(0, 1)*2)-1 for x in range(sig_len)]
        tx_signal = tx_signal * 2
        add = blocks.add_cc()
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq   = gr.vector_sink_f()
        sink_detect = gr.vector_sink_b()
        self.tb.connect(gr.vector_source_c(tx_signal), (add, 0))
        self.tb.connect(analog.noise_source_c(analog.GR_GAUSSIAN, .01), (add, 1))
        self.tb.connect(add, sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        sig1_detect = sink_detect.data()[0:len(tx_signal)/2]
        sig2_detect = sink_detect.data()[len(tx_signal)/2:]
        self.assertTrue(abs(sig1_detect.index(1) - (n_zeros + fft_len + cp_len)) < cp_len)
        self.assertTrue(abs(sig2_detect.index(1) - (n_zeros + fft_len + cp_len)) < cp_len)
        self.assertEqual(numpy.sum(sig1_detect), 1)
        self.assertEqual(numpy.sum(sig2_detect), 1)


    def test_002_freq (self):
        """ Add a fine frequency offset and see if that get's detected properly """
        fft_len = 32
        cp_len = 4
        freq_offset = 0.1 # Must stay < 2*pi/fft_len = 0.196 (otherwise, it's coarse)
        sig_len = (fft_len + cp_len) * 10
        sync_symbol = [(random.randint(0, 1)*2)-1 for x in range(fft_len/2)] * 2
        tx_signal = sync_symbol[-cp_len:] + \
                    sync_symbol + \
                    [(random.randint(0, 1)*2)-1 for x in range(sig_len)]
        mult = blocks.multiply_cc()
        add = blocks.add_cc()
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq   = gr.vector_sink_f()
        sink_detect = gr.vector_sink_b()
        self.tb.connect(gr.vector_source_c(tx_signal), (mult, 0), (add, 0))
        self.tb.connect(analog.sig_source_c(2 * numpy.pi, analog.GR_SIN_WAVE, freq_offset, 1.0), (mult, 1))
        self.tb.connect(analog.noise_source_c(analog.GR_GAUSSIAN, .01), (add, 1))
        self.tb.connect(add, sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        phi_hat = sink_freq.data()[sink_detect.data().index(1)]
        est_freq_offset = 2 * phi_hat / fft_len
        self.assertAlmostEqual(est_freq_offset, freq_offset, places=2)


    def test_003_multiburst (self):
        """ Send several bursts, see if the number of detects is correct.
        Burst lengths and content are random.
        """
        n_bursts = 42
        fft_len = 32
        cp_len = 4
        tx_signal = []
        for i in xrange(n_bursts):
            sync_symbol = [(random.randint(0, 1)*2)-1 for x in range(fft_len/2)] * 2
            tx_signal += [0,] * random.randint(0, 2*fft_len) + \
                         sync_symbol[-cp_len:] + \
                         sync_symbol + \
                         [(random.randint(0, 1)*2)-1 for x in range(fft_len * random.randint(5,23))]
        add = blocks.add_cc()
        sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        sink_freq   = gr.vector_sink_f()
        sink_detect = gr.vector_sink_b()
        self.tb.connect(gr.vector_source_c(tx_signal), (add, 0))
        self.tb.connect(analog.noise_source_c(analog.GR_GAUSSIAN, .005), (add, 1))
        self.tb.connect(add, sync)
        self.tb.connect((sync, 0), sink_freq)
        self.tb.connect((sync, 1), sink_detect)
        self.tb.run()
        self.assertEqual(numpy.sum(sink_detect.data()), n_bursts,
                msg="""Because of statistics, it is possible (though unlikely)
that the number of detected bursts differs slightly. If the number of detects is
off by one or two, run the test again and see what happen.
Detection error was: %d """ % (numpy.sum(sink_detect.data()) - n_bursts)
        )

    # FIXME ofdm_mod is currently not working
    #def test_004_ofdm_packets (self):
        #"""
        #Send several bursts, see if the number of detects is correct.
        #Burst lengths and content are random.
        #"""
        #n_bursts = 42
        #fft_len = 64
        #cp_len = 12
        #tx_signal = []
        #packets = []
        #tagname = "length"
        #min_packet_length = 100
        #max_packet_length = 100
        #sync_sequence = [random.randint(0, 1)*2-1 for x in range(fft_len/2)]
        #for i in xrange(n_bursts):
            #packet_length = random.randint(min_packet_length,
                                           #max_packet_length+1)
            #packet = [random.randint(0, 255) for i in range(packet_length)]
            #packets.append(packet)
        #data, tags = tagged_streams.packets_to_vectors(
            #packets, tagname, vlen=1)
        #total_length = len(data)

        #src = gr.vector_source_b(data, False, 1, tags)
        #mod = ofdm_tx(
                #fft_len=fft_len,
                #cp_len=cp_len,
                #length_tag_name=tagname,
                #occupied_carriers=(range(1, 27) + range(38, 64),),
                #pilot_carriers=((0,),),
                #pilot_symbols=((100,),),
        #)
        #rate_in = 16000
        #rate_out = 48000
        #ratio = float(rate_out) / rate_in
        #throttle1 = gr.throttle(gr.sizeof_gr_complex, rate_in)
        #sink_countbursts = gr.vector_sink_c()
        #head = gr.head(gr.sizeof_gr_complex, int(total_length * ratio*2))
        #add = gr.add_cc()
        #sync = digital.ofdm_sync_sc_cfb(fft_len, cp_len)
        #sink_freq   = gr.vector_sink_f()
        #sink_detect = gr.vector_sink_b()
        #noise_level = 0.01
        #noise = gr.noise_source_c(gr.GR_GAUSSIAN, noise_level)
        #self.tb.connect(src, mod, gr.null_sink(gr.sizeof_gr_complex))
        #self.tb.connect(insert_zeros, sink_countbursts)
        #self.tb.connect(noise, (add, 1))
        #self.tb.connect(add, sync)
        #self.tb.connect((sync, 0), sink_freq)
        #self.tb.connect((sync, 1), sink_detect)
        #self.tb.run()
        #count_data = sink_countbursts.data()
        #count_tags = sink_countbursts.tags()
        #burstcount = tagged_streams.count_bursts(count_data, count_tags, tagname)
        #self.assertEqual(numpy.sum(sink_detect.data()), burstcount)


if __name__ == '__main__':
    #gr_unittest.run(qa_ofdm_sync_sc_cfb, "qa_ofdm_sync_sc_cfb.xml")
    gr_unittest.run(qa_ofdm_sync_sc_cfb)

