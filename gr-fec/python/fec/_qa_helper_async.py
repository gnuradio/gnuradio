#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
# Copyright 2023 Daniel Estevez <daniel@destevez.net>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# This is similar to _qa_helper.py but uses async_encoder and async_decoder
# instead of extended_encoder and extended_decoder.


import numpy as np

from gnuradio import gr, blocks, fec, pdu
# Must use absolute import here because this file is not installed as part
# of the module
from gnuradio.fec import async_encoder, async_decoder
import pmt

from _qa_helper import map_bb


class _qa_helper_async(gr.top_block):

    def __init__(self, frame_size, enc, dec, packed, rev_pack):
        gr.top_block.__init__(self, "_qa_helper_async")

        self.enc = enc
        self.dec = dec
        self.frame_size = frame_size
        # These options are for the decoder only. The encoder is always run in
        # unpacked mode.
        self.packed = packed
        self.rev_pack = rev_pack

        self.async_encoder = async_encoder(enc)
        self.async_decoder = async_decoder(
            dec, packed=packed, rev_pack=rev_pack)

        num_frames = 64
        frame_size_bits = 8 * frame_size
        data_in = np.random.randint(
            2, size=frame_size_bits * num_frames, dtype='uint8')
        tags = [
            gr.tag_utils.python_to_tag((
                frame_size_bits * j, pmt.intern('packet_len'),
                pmt.from_long(frame_size_bits), pmt.intern('src')))
            for j in range(num_frames)]
        self.src = blocks.vector_source_b(data_in, False, tags=tags)
        self.tagged_to_pdu_enc = pdu.tagged_stream_to_pdu(
            gr.types.byte_t, 'packet_len')
        self.pdu_to_tagged_enc = pdu.pdu_to_tagged_stream(
            gr.types.byte_t, 'packet_len')
        self.map = map_bb([-1, 1])
        self.to_float = blocks.char_to_float(1)
        self.tagged_to_pdu_dec = pdu.tagged_stream_to_pdu(
            gr.types.float_t, 'packet_len')
        self.pdu_to_tagged_dec = pdu.pdu_to_tagged_stream(
            gr.types.byte_t, 'packet_len')
        self.snk_input = blocks.vector_sink_b()
        self.snk_output = blocks.vector_sink_b()
        if packed:
            self.unpack_decoder = blocks.packed_to_unpacked_bb(
                1, gr.GR_LSB_FIRST if rev_pack else gr.GR_MSB_FIRST)

        self.connect(self.src, self.tagged_to_pdu_enc)
        self.msg_connect((self.tagged_to_pdu_enc, 'pdus'),
                         (self.async_encoder, 'in'))
        self.msg_connect((self.async_encoder, 'out'),
                         (self.pdu_to_tagged_enc, 'pdus'))
        self.connect(
            self.pdu_to_tagged_enc, self.map, self.to_float,
            self.tagged_to_pdu_dec)
        self.msg_connect((self.tagged_to_pdu_dec, 'pdus'),
                         (self.async_decoder, 'in'))
        self.msg_connect((self.async_decoder, 'out'),
                         (self.pdu_to_tagged_dec, 'pdus'))
        self.connect(self.src, self.snk_input)
        if packed:
            self.connect(
                self.pdu_to_tagged_dec, self.unpack_decoder, self.snk_output)
        else:
            self.connect(self.pdu_to_tagged_dec, self.snk_output)


if __name__ == '__main__':
    frame_size = 30
    enc = fec.dummy_encoder_make(frame_size * 8)
    dec = fec.dummy_decoder.make(frame_size * 8)

    for packed in [True, False]:
        for rev_pack in [True, False]:
            tb = _qa_helper_async(frame_size, enc, dec, packed, rev_pack)
            tb.run()

            errs = 0
            for i, o in zip(tb.snk_input.data(), tb.snk_output.data()):
                if i - o != 0:
                    errs += 1

            if errs == 0:
                print("Decoded properly")
            else:
                print("Problem Decoding")
