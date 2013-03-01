#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

import numpy
from gnuradio import gr

class square3_ff(gr.sync_block):
    " Squaring block "
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name = "square3_ff",
            in_sig = [numpy.float32], # Input signature: 1 float at a time
            out_sig = [numpy.float32], # Output signature: 1 float at a time
        )

    def work(self, input_items, output_items):
        """ Notes:
        - You must not forget the [:] at the output items, otherwise
          stuff doesn't truly get copied to the output
        - Both input_ and output_items[N] are numpy arrays, so you can
          do fancy stuff like multiplying them elementwise
        - noutput_items and ninput_items are implicit in the length of
          output_items and input_items, respectively
        """
        output_items[0][:] = input_items[0] * input_items[0]
        return len(output_items[0])

