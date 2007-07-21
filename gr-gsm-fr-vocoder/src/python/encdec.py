#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import audio
from gnuradio.vocoder import gsm_full_rate

def build_graph():
    fg = gr.flow_graph()
    src = audio.source(8000)
    src_scale = gr.multiply_const_ff(32767)
    f2s = gr.float_to_short ()
    enc = gsm_full_rate.encode_sp()
    dec = gsm_full_rate.decode_ps()
    s2f = gr.short_to_float ()
    sink_scale = gr.multiply_const_ff(1.0/32767.)
    sink = audio.sink(8000)
    fg.connect(src, src_scale, f2s, enc, dec, s2f, sink_scale, sink)
    return fg

if __name__ == '__main__':
    fg = build_graph()
    fg.start()
    raw_input ('Press Enter to exit: ')
    fg.stop()
