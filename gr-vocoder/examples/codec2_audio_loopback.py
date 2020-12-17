#!/usr/bin/env python
#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import audio
from gnuradio import blocks
from gnuradio import vocoder
from gnuradio.vocoder import codec2

def build_graph():
    tb = gr.top_block()
    src = audio.source(8000)
    src_scale = blocks.multiply_const_ff(32767)
    f2s = blocks.float_to_short()
    enc = vocoder.codec2_encode_sp(codec2.MODE_2400)
    dec = vocoder.codec2_decode_ps(codec2.MODE_2400)
    s2f = blocks.short_to_float()
    sink_scale = blocks.multiply_const_ff(1.0 / 32767.)
    sink = audio.sink(8000)
    tb.connect(src, src_scale, f2s, enc, dec, s2f, sink_scale, sink)
    return tb

if __name__ == '__main__':
    tb = build_graph()
    tb.start()
    input ('Press Enter to exit: ')
    tb.stop()
    tb.wait()
