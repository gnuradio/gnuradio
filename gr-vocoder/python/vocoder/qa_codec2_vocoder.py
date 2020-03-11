#!/usr/bin/env python
#
# Copyright 2011,2013,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, vocoder, blocks
from gnuradio.vocoder import codec2

class test_codec2_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_mode_2400_encoder(self):
        # compresses a
        # 16-bit 8 kHz (128 kbit/s) PCM stream
        # to 2400 bit/s
        enc = vocoder.codec2_encode_sp(codec2.MODE_2400)
        samples_per_frame = enc.relative_rate_d()
        data = samples_per_frame*(100,200,300,400,500,600,700,800)
        src = blocks.vector_source_s(data)
        snk = blocks.vector_sink_b(vlen=48)

        expected_length = ((len(data) * 16)*2400)//128000
        self.tb.connect(src,enc,snk)
        self.tb.run()
        result = snk.data()
        self.assertEqual(expected_length, len(result))

    def test001_mode_1600_encoder(self):
        enc = vocoder.codec2_encode_sp(codec2.MODE_1600)
        samples_per_frame = enc.relative_rate_d()
        bits_per_frame = enc.output_signature().sizeof_stream_item(0)
        data = samples_per_frame*(100,200,300,400,500,600,700,800)
        src = blocks.vector_source_s(data)
        snk = blocks.vector_sink_b(vlen=bits_per_frame)

        expected_length = ((len(data) * 16)*1600)//128000
        self.tb.connect(src,enc,snk)
        self.tb.run()
        result = snk.data()
        self.assertEqual(expected_length, len(result))

    def test001_mode_1400_encoder(self):
        enc = vocoder.codec2_encode_sp(codec2.MODE_1400)
        samples_per_frame = enc.relative_rate_d()
        bits_per_frame = enc.output_signature().sizeof_stream_item(0)
        data = samples_per_frame*(100,200,300,400,500,600,700,800)
        src = blocks.vector_source_s(data)
        snk = blocks.vector_sink_b(vlen=bits_per_frame)

        expected_length = ((len(data) * 16)*1400)//128000
        self.tb.connect(src,enc,snk)
        self.tb.run()
        result = snk.data()
        self.assertEqual(expected_length, len(result))

if __name__ == '__main__':
    gr_unittest.run(test_codec2_vocoder)
