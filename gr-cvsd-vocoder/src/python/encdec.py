#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, blks
from gnuradio import audio
from gnuradio.vocoder import cvsd_vocoder

def build_graph():
    sample_rate = 8000
    scale_factor = 32000
    
    fg = gr.flow_graph()
    src = audio.source(sample_rate, "plughw:0,0")
    src_scale = gr.multiply_const_ff(scale_factor)

    interp = blks.rational_resampler_fff(fg, 8, 1)
    f2s = gr.float_to_short ()

    enc = cvsd_vocoder.encode_sb()
    dec = cvsd_vocoder.decode_bs()

    s2f = gr.short_to_float ()
    decim = blks.rational_resampler_fff(fg, 1, 8)

    sink_scale = gr.multiply_const_ff(1.0/scale_factor)
    sink = audio.sink(sample_rate, "plughw:0,0")

    fg.connect(src, src_scale, interp, f2s, enc)
    fg.connect(enc, dec, s2f, decim, sink_scale, sink)

    if 0: # debug
        fg.connect(src, gr.file_sink(gr.sizeof_float, "source.dat"))
        fg.connect(src_scale, gr.file_sink(gr.sizeof_float, "src_scale.dat"))
        fg.connect(interp, gr.file_sink(gr.sizeof_float, "interp.dat"))
        fg.connect(f2s, gr.file_sink(gr.sizeof_short, "f2s.dat"))
        fg.connect(enc, gr.file_sink(gr.sizeof_char,  "enc.dat"))
        fg.connect(dec, gr.file_sink(gr.sizeof_short, "dec.dat"))
        fg.connect(s2f, gr.file_sink(gr.sizeof_float, "s2f.dat"))
        fg.connect(decim, gr.file_sink(gr.sizeof_float, "decim.dat"))
        fg.connect(sink_scale, gr.file_sink(gr.sizeof_float, "sink_scale.dat"))
        
    return fg

if __name__ == '__main__':
    fg = build_graph()
    fg.start()
    raw_input ('Press Enter to exit: ')
    fg.stop()
