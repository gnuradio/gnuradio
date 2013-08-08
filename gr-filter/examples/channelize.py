#!/usr/bin/env python
#
# Copyright 2009,2012,2013 Free Software Foundation, Inc.
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
import sys, time

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

try:
    import scipy
    from scipy import fftpack
except ImportError:
    sys.stderr.write("Error: Program requires scipy (see: www.scipy.org).\n")
    sys.exit(1)

try:
    import pylab
    from pylab import mlab
except ImportError:
    sys.stderr.write("Error: Program requires matplotlib (see: matplotlib.sourceforge.net).\n")
    sys.exit(1)

class pfb_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self._N = 2000000        # number of samples to use
        self._fs = 1000          # initial sampling rate
        self._M = M = 9          # Number of channels to channelize
        self._ifs = M*self._fs   # initial sampling rate

        # Create a set of taps for the PFB channelizer
        self._taps = filter.firdes.low_pass_2(1, self._ifs, 475.50, 50,
                                              attenuation_dB=100,
                                              window=filter.firdes.WIN_BLACKMAN_hARRIS)

        # Calculate the number of taps per channel for our own information
        tpc = scipy.ceil(float(len(self._taps)) /  float(self._M))
        print "Number of taps:     ", len(self._taps)
        print "Number of channels: ", self._M
        print "Taps per channel:   ", tpc

        # Create a set of signals at different frequencies
        #   freqs lists the frequencies of the signals that get stored
        #   in the list "signals", which then get summed together
        self.signals = list()
        self.add = blocks.add_cc()
        freqs = [-70, -50, -30, -10, 10, 20, 40, 60, 80]
        for i in xrange(len(freqs)):
            f = freqs[i] + (M/2-M+i+1)*self._fs
            self.signals.append(analog.sig_source_c(self._ifs, analog.GR_SIN_WAVE, f, 1))
            self.connect(self.signals[i], (self.add,i))

        self.head = blocks.head(gr.sizeof_gr_complex, self._N)

        # Construct the channelizer filter
        self.pfb = filter.pfb.channelizer_ccf(self._M, self._taps, 1)

        # Construct a vector sink for the input signal to the channelizer
        self.snk_i = blocks.vector_sink_c()

        # Connect the blocks
        self.connect(self.add, self.head, self.pfb)
        self.connect(self.add, self.snk_i)

        # Use this to play with the channel mapping
        #self.pfb.set_channel_map([5,6,7,8,0,1,2,3,4])

        # Create a vector sink for each of M output channels of the filter and connect it
        self.snks = list()
        for i in xrange(self._M):
            self.snks.append(blocks.vector_sink_c())
            self.connect((self.pfb, i), self.snks[i])


def main():
    tstart = time.time()

    tb = pfb_top_block()
    tb.run()

    tend = time.time()
    print "Run time: %f" % (tend - tstart)

    if 1:
        fig_in = pylab.figure(1, figsize=(16,9), facecolor="w")
        fig1 = pylab.figure(2, figsize=(16,9), facecolor="w")
        fig2 = pylab.figure(3, figsize=(16,9), facecolor="w")

        Ns = 1000
        Ne = 10000

        fftlen = 8192
        winfunc = scipy.blackman
        fs = tb._ifs

        # Plot the input signal on its own figure
        d = tb.snk_i.data()[Ns:Ne]
        spin_f = fig_in.add_subplot(2, 1, 1)

        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_in = 10.0*scipy.log10(abs(X))
        f_in = scipy.arange(-fs/2.0, fs/2.0, fs/float(X_in.size))
        pin_f = spin_f.plot(f_in, X_in, "b")
        spin_f.set_xlim([min(f_in), max(f_in)+1])
        spin_f.set_ylim([-200.0, 50.0])

        spin_f.set_title("Input Signal", weight="bold")
        spin_f.set_xlabel("Frequency (Hz)")
        spin_f.set_ylabel("Power (dBW)")


        Ts = 1.0/fs
        Tmax = len(d)*Ts

        t_in = scipy.arange(0, Tmax, Ts)
        x_in = scipy.array(d)
        spin_t = fig_in.add_subplot(2, 1, 2)
        pin_t = spin_t.plot(t_in, x_in.real, "b")
        pin_t = spin_t.plot(t_in, x_in.imag, "r")

        spin_t.set_xlabel("Time (s)")
        spin_t.set_ylabel("Amplitude")

        Ncols = int(scipy.floor(scipy.sqrt(tb._M)))
        Nrows = int(scipy.floor(tb._M / Ncols))
        if(tb._M % Ncols != 0):
            Nrows += 1

        # Plot each of the channels outputs. Frequencies on Figure 2 and
        # time signals on Figure 3
        fs_o = tb._fs
        Ts_o = 1.0/fs_o
        Tmax_o = len(d)*Ts_o
        for i in xrange(len(tb.snks)):
            # remove issues with the transients at the beginning
            # also remove some corruption at the end of the stream
            #    this is a bug, probably due to the corner cases
            d = tb.snks[i].data()[Ns:Ne]

            sp1_f = fig1.add_subplot(Nrows, Ncols, 1+i)
            X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs_o,
                              window = lambda d: d*winfunc(fftlen),
                              scale_by_freq=True)
            X_o = 10.0*scipy.log10(abs(X))
            f_o = scipy.arange(-fs_o/2.0, fs_o/2.0, fs_o/float(X_o.size))
            p2_f = sp1_f.plot(f_o, X_o, "b")
            sp1_f.set_xlim([min(f_o), max(f_o)+1])
            sp1_f.set_ylim([-200.0, 50.0])

            sp1_f.set_title(("Channel %d" % i), weight="bold")
            sp1_f.set_xlabel("Frequency (Hz)")
            sp1_f.set_ylabel("Power (dBW)")

            x_o = scipy.array(d)
            t_o = scipy.arange(0, Tmax_o, Ts_o)
            sp2_o = fig2.add_subplot(Nrows, Ncols, 1+i)
            p2_o = sp2_o.plot(t_o, x_o.real, "b")
            p2_o = sp2_o.plot(t_o, x_o.imag, "r")
            sp2_o.set_xlim([min(t_o), max(t_o)+1])
            sp2_o.set_ylim([-2, 2])

            sp2_o.set_title(("Channel %d" % i), weight="bold")
            sp2_o.set_xlabel("Time (s)")
            sp2_o.set_ylabel("Amplitude")

        pylab.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

