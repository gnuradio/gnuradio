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

from gnuradio import gr, filter
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

try:
    import scipy
except ImportError:
    print "Error: could not import scipy (http://www.scipy.org/)"
    sys.exit(1)

try:
    import pylab
except ImportError:
    print "Error: could not import pylab (http://matplotlib.sourceforge.net/)"
    sys.exit(1)

class example_fft_filter_ccc(gr.top_block):
    def __init__(self, N, fs, bw0, bw1, tw, atten, D):
        gr.top_block.__init__(self)

        self._nsamps = N
        self._fs = fs
        self._bw0 = bw0
        self._bw1 = bw1
        self._tw = tw
        self._at = atten
        self._decim = D
        taps = filter.firdes.complex_band_pass_2(1, self._fs,
                                                 self._bw0, self._bw1,
                                                 self._tw, self._at)
        print "Num. Taps: ", len(taps)

        self.src  = analog.noise_source_c(analog.GR_GAUSSIAN, 1)
        self.head = blocks.head(gr.sizeof_gr_complex, self._nsamps)

        self.filt0 = filter.fft_filter_ccc(self._decim, taps)

        self.vsnk_src = blocks.vector_sink_c()
        self.vsnk_out = blocks.vector_sink_c()

        self.connect(self.src, self.head, self.vsnk_src)
        self.connect(self.head, self.filt0, self.vsnk_out)

def main():
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-N", "--nsamples", type="int", default=10000,
                      help="Number of samples to process [default=%default]")
    parser.add_option("-s", "--samplerate", type="eng_float", default=8000,
                      help="System sample rate [default=%default]")
    parser.add_option("-S", "--start-pass", type="eng_float", default=1000,
                      help="Start of Passband [default=%default]")
    parser.add_option("-E", "--end-pass", type="eng_float", default=2000,
                      help="End of Passband [default=%default]")
    parser.add_option("-T", "--transition", type="eng_float", default=100,
                      help="Transition band [default=%default]")
    parser.add_option("-A", "--attenuation", type="eng_float", default=80,
                      help="Stopband attenuation [default=%default]")
    parser.add_option("-D", "--decimation", type="int", default=1,
                      help="Decmation factor [default=%default]")
    (options, args) = parser.parse_args ()

    put = example_fft_filter_ccc(options.nsamples,
                                 options.samplerate,
                                 options.start_pass,
                                 options.end_pass,
                                 options.transition,
                                 options.attenuation,
                                 options.decimation)
    put.run()

    data_src = scipy.array(put.vsnk_src.data())
    data_snk = scipy.array(put.vsnk_out.data())

    # Plot the signals PSDs
    nfft = 1024
    f1 = pylab.figure(1, figsize=(12,10))
    s1 = f1.add_subplot(1,1,1)
    s1.psd(data_src, NFFT=nfft, noverlap=nfft/4,
           Fs=options.samplerate)
    s1.psd(data_snk, NFFT=nfft, noverlap=nfft/4,
           Fs=options.samplerate)

    f2 = pylab.figure(2, figsize=(12,10))
    s2 = f2.add_subplot(1,1,1)
    s2.plot(data_src)
    s2.plot(data_snk.real, 'g')

    pylab.show()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

