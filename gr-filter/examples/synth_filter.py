#!/usr/bin/env python
#
# Copyright 2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import division
from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import filter
from gnuradio import blocks
import sys
import numpy

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

try:
    from matplotlib import pyplot
except ImportError:
    sys.stderr.write("Error: Program requires matplotlib (see: matplotlib.sourceforge.net).\n")
    sys.exit(1)

def main():
    N = 1000000
    fs = 8000

    freqs = [100, 200, 300, 400, 500]
    nchans = 7

    sigs = list()
    for fi in freqs:
        s = analog.sig_source_c(fs, analog.GR_SIN_WAVE, fi, 1)
        sigs.append(s)

    taps = filter.firdes.low_pass_2(len(freqs), fs,
                                    fs/float(nchans)/2, 100, 100)
    print("Num. Taps = %d (taps per filter = %d)" % (len(taps),
                                                     len(taps) / nchans))
    filtbank = filter.pfb_synthesizer_ccf(nchans, taps)

    head = blocks.head(gr.sizeof_gr_complex, N)
    snk = blocks.vector_sink_c()

    tb = gr.top_block()
    tb.connect(filtbank, head, snk)

    for i,si in enumerate(sigs):
        tb.connect(si, (filtbank, i))

    tb.run()

    if 1:
        f1 = pyplot.figure(1)
        s1 = f1.add_subplot(1,1,1)
        s1.plot(snk.data()[1000:])

        fftlen = 2048
        f2 = pyplot.figure(2)
        s2 = f2.add_subplot(1,1,1)
        winfunc = numpy.blackman
        s2.psd(snk.data()[10000:], NFFT=fftlen,
               Fs = nchans*fs,
               noverlap=fftlen / 4,
               window = lambda d: d*winfunc(fftlen))

        pyplot.show()

if __name__ == "__main__":
    main()
