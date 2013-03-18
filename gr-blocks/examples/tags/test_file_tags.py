#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
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
from gnuradio import blocks
import sys

try:
    import scipy
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)

def main():
    data = scipy.arange(0, 32000, 1).tolist()
    trig = 100*[0,] + 100*[1,]

    src = blocks.vector_source_s(data, True)
    trigger = blocks.vector_source_s(trig, True)

    thr = blocks.throttle(gr.sizeof_short, 10e3)
    ann = blocks.annotator_alltoall(1000000, gr.sizeof_short)
    tagger = blocks.burst_tagger(gr.sizeof_short)

    fsnk = blocks.tagged_file_sink(gr.sizeof_short, 1)

    tb = gr.top_block()
    tb.connect(src, thr, (tagger, 0))
    tb.connect(trigger, (tagger, 1))
    tb.connect(tagger, fsnk)

    tb.run()

if __name__ == "__main__":
    main()


