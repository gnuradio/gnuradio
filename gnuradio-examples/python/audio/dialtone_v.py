#!/usr/bin/env python

# Copyright 2006 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, audio
from math import pi, sin

"""
This test script demonstrates the use of element-wise vector processing
vs. stream processing.  The example is artificial in that the stream
version in dial_tone.py is the normal way to do it; in addition, the
envelope processing here is just for demo purposes and isn't needed.
"""

# For testing different buffer sizes
rate = 48000

fg = gr.flow_graph()

# Two streams of floats
a = gr.sig_source_f(rate, gr.GR_SIN_WAVE, 350, 0.5, 0.0);
b = gr.sig_source_f(rate, gr.GR_SIN_WAVE, 440, 0.5, 0.0);

# Turn them into vectors of length 'size'
av = gr.stream_to_vector(gr.sizeof_float, rate)
bv = gr.stream_to_vector(gr.sizeof_float, rate)

# Make a vector adder for float vectors
adder = gr.add_vff(rate)

# Make a 1 Hz sine envelope
envelope = [sin(2*pi*x/rate)*0.5 for x in range(rate)]
multiplier = gr.multiply_const_vff(envelope)

# Make an offset adder
offset = gr.add_const_vff((0.5,)*rate)

# Turn the vector back into a stream of floats
result = gr.vector_to_stream(gr.sizeof_float, rate)

# Play it
sink = audio.sink(rate)

fg.connect(a, av)
fg.connect(b, bv)
fg.connect(av, (adder, 0))
fg.connect(bv, (adder, 1))
fg.connect(adder, multiplier, offset, result, sink)

try:
    fg.run()
except KeyboardInterrupt:
    pass
