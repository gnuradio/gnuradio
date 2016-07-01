#!/usr/bin/env python
#
# Copyright 2005-2007,2013 Free Software Foundation, Inc.
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

import time
import random
from optparse import OptionParser
from gnuradio import gr
from gnuradio import blocks, filter
from gnuradio.eng_option import eng_option

def make_random_complex_tuple(L):
    result = []
    for x in range(L):
        result.append(complex(random.uniform(-1000,1000),
                              random.uniform(-1000,1000)))
    return tuple(result)

def benchmark(name, creator, dec, ntaps, total_test_size, block_size):
    block_size = 32768

    tb = gr.top_block()
    taps = make_random_complex_tuple(ntaps)
    src = blocks.vector_source_c(make_random_complex_tuple(block_size), True)
    head = blocks.head(gr.sizeof_gr_complex, int(total_test_size))
    op = creator(dec, taps)
    dst = blocks.null_sink(gr.sizeof_gr_complex)
    tb.connect(src, head, op, dst)
    start = time.time()
    tb.run()
    stop = time.time()
    delta = stop - start
    print "%16s: taps: %4d  input: %4g, time: %6.3f  taps/sec: %10.4g" % (
        name, ntaps, total_test_size, delta, ntaps*total_test_size/delta)

def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-n", "--ntaps", type="int", default=256)
    parser.add_option("-t", "--total-input-size", type="eng_float", default=40e6)
    parser.add_option("-b", "--block-size", type="intx", default=50000)
    parser.add_option("-d", "--decimation", type="int", default=1)
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    ntaps = options.ntaps
    total_input_size = options.total_input_size
    block_size = options.block_size
    dec = options.decimation

    benchmark("filter.fir_filter_ccc", filter.fir_filter_ccc,
              dec, ntaps, total_input_size, block_size)
    benchmark("filter.fft_filter_ccc", filter.fft_filter_ccc,
              dec, ntaps, total_input_size, block_size)

if __name__ == '__main__':
    main()
