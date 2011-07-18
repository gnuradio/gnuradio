#!/usr/bin/env python
#
# Copyright 2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gr, blks2
from gnuradio import audio
from gnuradio import vocoder

def build_graph():
    sample_rate = 8000
    scale_factor = 32000
    
    tb = gr.top_block()
    src = audio.source(sample_rate, "plughw:0,0")
    src_scale = gr.multiply_const_ff(scale_factor)

    interp = blks2.rational_resampler_fff(8, 1)
    f2s = gr.float_to_short ()

    enc = vocoder.cvsd_encode_sb()
    dec = vocoder.cvsd_decode_bs()

    s2f = gr.short_to_float ()
    decim = blks2.rational_resampler_fff(1, 8)

    sink_scale = gr.multiply_const_ff(1.0/scale_factor)
    sink = audio.sink(sample_rate, "plughw:0,0")

    tb.connect(src, src_scale, interp, f2s, enc)
    tb.connect(enc, dec, s2f, decim, sink_scale, sink)

    if 0: # debug
        tb.conect(src, gr.file_sink(gr.sizeof_float, "source.dat"))
        tb.conect(src_scale, gr.file_sink(gr.sizeof_float, "src_scale.dat"))
        tb.conect(interp, gr.file_sink(gr.sizeof_float, "interp.dat"))
        tb.conect(f2s, gr.file_sink(gr.sizeof_short, "f2s.dat"))
        tb.conect(enc, gr.file_sink(gr.sizeof_char,  "enc.dat"))
        tb.conect(dec, gr.file_sink(gr.sizeof_short, "dec.dat"))
        tb.conect(s2f, gr.file_sink(gr.sizeof_float, "s2f.dat"))
        tb.conect(decim, gr.file_sink(gr.sizeof_float, "decim.dat"))
        tb.conect(sink_scale, gr.file_sink(gr.sizeof_float, "sink_scale.dat"))
        
    return tb

if __name__ == '__main__':
    tb = build_graph()
    tb.start()
    raw_input ('Press Enter to exit: ')
    tb.stop()
    tb.wait()
