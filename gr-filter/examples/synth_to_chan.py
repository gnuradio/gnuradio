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
from gnuradio import blocks
from gnuradio import filter
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
    fmtx = list()
    for fi in freqs:
        s = analog.sig_source_f(fs, analog.GR_SIN_WAVE, fi, 1)
        fm = analog.nbfm_tx(fs, 4*fs, max_dev=10000, tau=75e-6, fh=0.925*(4*fs)/2.0)
        sigs.append(s)
        fmtx.append(fm)

    syntaps = filter.firdes.low_pass_2(len(freqs), fs, fs/float(nchans)/2, 100, 100)
    print "Synthesis Num. Taps = %d (taps per filter = %d)" % (len(syntaps),
                                                               len(syntaps)/nchans)
    chtaps = filter.firdes.low_pass_2(len(freqs), fs, fs/float(nchans)/2, 100, 100)
    print "Channelizer Num. Taps = %d (taps per filter = %d)" % (len(chtaps),
                                                                 len(chtaps)/nchans)
    filtbank = filter.pfb_synthesizer_ccf(nchans, syntaps)
    channelizer = filter.pfb.channelizer_ccf(nchans, chtaps)

    noise_level = 0.01
    head = blocks.head(gr.sizeof_gr_complex, N)
    noise = analog.noise_source_c(analog.GR_GAUSSIAN, noise_level)
    addnoise = blocks.add_cc()
    snk_synth = blocks.vector_sink_c()

    tb = gr.top_block()

    tb.connect(noise, (addnoise,0))
    tb.connect(filtbank, head, (addnoise, 1))
    tb.connect(addnoise, channelizer)
    tb.connect(addnoise, snk_synth)

    snk = list()
    for i,si in enumerate(sigs):
        tb.connect(si, fmtx[i], (filtbank, i))

    for i in xrange(nchans):
        snk.append(blocks.vector_sink_c())
        tb.connect((channelizer, i), snk[i])

    tb.run()

    if 1:
        channel = 1
        data = snk[channel].data()[1000:]

        f1 = pylab.figure(1)
        s1 = f1.add_subplot(1,1,1)
        s1.plot(data[10000:10200] )
        s1.set_title(("Output Signal from Channel %d" % channel))

        fftlen = 2048
        winfunc = scipy.blackman
        #winfunc = scipy.hamming

        f2 = pylab.figure(2)
        s2 = f2.add_subplot(1,1,1)
        s2.psd(data, NFFT=fftlen,
               Fs = nchans*fs,
               noverlap=fftlen/4,
               window = lambda d: d*winfunc(fftlen))
        s2.set_title(("Output PSD from Channel %d" % channel))

        f3 = pylab.figure(3)
        s3 = f3.add_subplot(1,1,1)
        s3.psd(snk_synth.data()[1000:], NFFT=fftlen,
               Fs = nchans*fs,
               noverlap=fftlen/4,
               window = lambda d: d*winfunc(fftlen))
        s3.set_title("Output of Synthesis Filter")

        pylab.show()

if __name__ == "__main__":
    main()
