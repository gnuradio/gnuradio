#!/usr/bin/env python
#
# Copyright 2007,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import division
from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import audio
from gnuradio import blocks
from gnuradio import filter
from gnuradio import vocoder

def build_graph():
    sample_rate = 8000
    scale_factor = 32000

    tb = gr.top_block()
    src = audio.source(sample_rate, "plughw:0,0")
    src_scale = blocks.multiply_const_ff(scale_factor)

    interp = filter.rational_resampler_fff(8, 1)
    f2s = blocks.float_to_short()

    enc = vocoder.cvsd_encode_sb()
    dec = vocoder.cvsd_decode_bs()

    s2f = blocks.short_to_float()
    decim = filter.rational_resampler_fff(1, 8)

    sink_scale = blocks.multiply_const_ff(1.0 / scale_factor)
    sink = audio.sink(sample_rate, "plughw:0,0")

    tb.connect(src, src_scale, interp, f2s, enc)
    tb.connect(enc, dec, s2f, decim, sink_scale, sink)

    if 0: # debug
        tb.connect(src, blocks.file_sink(gr.sizeof_float, "source.dat"))
        tb.connect(src_scale, blocks.file_sink(gr.sizeof_float, "src_scale.dat"))
        tb.connect(interp, blocks.file_sink(gr.sizeof_float, "interp.dat"))
        tb.connect(f2s, blocks.file_sink(gr.sizeof_short, "f2s.dat"))
        tb.connect(enc, blocks.file_sink(gr.sizeof_char,  "enc.dat"))
        tb.connect(dec, blocks.file_sink(gr.sizeof_short, "dec.dat"))
        tb.connect(s2f, blocks.file_sink(gr.sizeof_float, "s2f.dat"))
        tb.connect(decim, blocks.file_sink(gr.sizeof_float, "decim.dat"))
        tb.connect(sink_scale, blocks.file_sink(gr.sizeof_float, "sink_scale.dat"))

    return tb

if __name__ == '__main__':
    tb = build_graph()
    tb.start()
    input ('Press Enter to exit: ')
    tb.stop()
    tb.wait()
