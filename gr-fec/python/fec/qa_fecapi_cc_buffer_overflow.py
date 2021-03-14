#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import blocks, analog, pdu

# This test tries to checks for the kind of buffer overflows in the
# FECAPI convolutional decoder that were fixed in #2965


class test_fecapi_cc_buffer_overflow(gr_unittest.TestCase):

    def run_with_frame_len(self, frame_len):
        self.tb = gr.top_block()
        noise = analog.noise_source_f(analog.GR_GAUSSIAN, 1, 0)
        head = blocks.head(gr.sizeof_float, self.n_frames * frame_len)
        tag = blocks.stream_to_tagged_stream(
            gr.sizeof_float, 1, frame_len, "packet_len")
        stream2pdu = pdu.tagged_stream_to_pdu(gr.types.float_t, 'packet_len')

        viterbi = fec.cc_decoder.make(
            frame_len // 2, 7, 2, [79, -109], 0, -1, fec.CC_TERMINATED, False)
        cc_decoder = fec.async_decoder(viterbi, False, False, frame_len)

        self.tb.connect(noise, head, tag, stream2pdu)
        self.tb.msg_connect((stream2pdu, 'pdus'), (cc_decoder, 'in'))

        self.tb.run()
        # the test is considered successful if it hasn't segfaulted or aborted

        self.tb = None

    def test_buffer_overflow_00(self):
        self.n_frames = 10

        for frame_len in range(1000, 2000):
            with self.subTest(frame_length=frame_len):
                self.run_with_frame_len(frame_len)


if __name__ == '__main__':
    gr_unittest.run(test_fecapi_cc_buffer_overflow)
