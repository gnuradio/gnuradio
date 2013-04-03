#!/usr/bin/env python
#
# Copyright 2010,2012,2013 Free Software Foundation, Inc.
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
from gnuradio import filter
from gnuradio import blocks
import sys

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

try:
    import scipy
except ImportError:
    sys.stderr.write("Error: Program requires scipy (see: www.scipy.org).\n")
    sys.exit(1)

try:
    import pylab
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
    print "Num. Taps = %d (taps per filter = %d)" % (len(taps),
                                                     len(taps)/nchans)
    filtbank = filter.pfb_synthesizer_ccf(nchans, taps)

    head = blocks.head(gr.sizeof_gr_complex, N)
    snk = blocks.vector_sink_c()

    tb = gr.top_block()
    tb.connect(filtbank, head, snk)

    for i,si in enumerate(sigs):
        tb.connect(si, (filtbank, i))

    tb.run()

    if 1:
        f1 = pylab.figure(1)
        s1 = f1.add_subplot(1,1,1)
        s1.plot(snk.data()[1000:])

        fftlen = 2048
        f2 = pylab.figure(2)
        s2 = f2.add_subplot(1,1,1)
        winfunc = scipy.blackman
        s2.psd(snk.data()[10000:], NFFT=fftlen,
               Fs = nchans*fs,
               noverlap=fftlen/4,
               window = lambda d: d*winfunc(fftlen))

        pylab.show()

if __name__ == "__main__":
    main()
