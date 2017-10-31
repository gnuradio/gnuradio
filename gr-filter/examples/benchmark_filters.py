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

from __future__ import print_function
from __future__ import unicode_literals
import time
import random
from argparse import ArgumentParser
from gnuradio import gr
from gnuradio import blocks, filter
from gnuradio.eng_arg import eng_float, intx

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
    print("%16s: taps: %4d  input: %4g, time: %6.3f  taps/sec: %10.4g" % (
        name, ntaps, total_test_size, delta, ntaps*total_test_size/delta))

def main():
    parser = ArgumentParser()
    parser.add_argument("-n", "--ntaps", type=int, default=256)
    parser.add_argument("-t", "--total-input-size", type=eng_float, default=40e6)
    parser.add_argument("-b", "--block-size", type=intx, default=50000)
    parser.add_argument("-d", "--decimation", type=int, default=1)
    args = parser.parse_args()

    benchmark("filter.fir_filter_ccc", filter.fir_filter_ccc, args.decimation,
              args.ntaps, args.total_input_size, args.block_size)
    benchmark("filter.fft_filter_ccc", filter.fft_filter_ccc, args.decimation,
              args.ntaps, args.total_input_size, args.block_size)

if __name__ == '__main__':
    main()
