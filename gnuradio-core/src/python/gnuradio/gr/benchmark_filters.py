#!/usr/bin/env python

import time
import random
from optparse import OptionParser
from gnuradio import gr
from gnuradio.eng_option import eng_option

def make_random_complex_tuple(L):
    result = []
    for x in range(L):
        result.append(complex(random.uniform(-1000,1000),
                              random.uniform(-1000,1000)))
    return tuple(result)
    
def benchmark(name, creator, dec, ntaps, total_test_size, block_size):
    block_size = 32768

    fg = gr.flow_graph()
    taps = make_random_complex_tuple(ntaps)
    src = gr.vector_source_c(make_random_complex_tuple(block_size), True)
    head = gr.head(gr.sizeof_gr_complex, int(total_test_size))
    op = creator(dec, taps)
    dst = gr.null_sink(gr.sizeof_gr_complex)
    fg.connect(src, head, op, dst)
    start = time.time()
    fg.run()
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

    benchmark("gr.fir_filter_ccc", gr.fir_filter_ccc,
              dec, ntaps, total_input_size, block_size)
    benchmark("gr.fft_filter_ccc", gr.fft_filter_ccc,
              dec, ntaps, total_input_size, block_size)

if __name__ == '__main__':
    main()
