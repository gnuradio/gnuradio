#!/usr/bin/env python

from gnuradio import gr, digital
from gnuradio.digital.utils import tagged_streams
import random

if __name__ == '__main__':
    tb = gr.top_block()
    fft_len = 64
    cp_len = 16
    length_tag_name = "length"
    src = gr.vector_source_c(
        [random.randint(0, 1)*2-1 for i in range(10*fft_len)], True, fft_len,
        tagged_streams.make_lengthtags((10,), (0,), length_tag_name))
    cyclic_prefixer = digital.ofdm_cyclic_prefixer(
        fft_len, fft_len+cp_len, 0, length_tag_name)
    sink = gr.null_sink(gr.sizeof_gr_complex)
    checker = digital.tagged_stream_check(gr.sizeof_gr_complex*fft_len, length_tag_name)
    checker2 = digital.tagged_stream_check(gr.sizeof_gr_complex*fft_len, length_tag_name)
    tb.connect(src, checker, cyclic_prefixer, sink)
    tb.run()

