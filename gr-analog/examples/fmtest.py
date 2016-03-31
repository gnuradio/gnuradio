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
from gnuradio import analog
from gnuradio import channels
import sys, math, time

try:
    import scipy
    from scipy import fftpack
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)

try:
    import pylab
except ImportError:
    print "Error: Program requires matplotlib (see: matplotlib.sourceforge.net)."
    sys.exit(1)


class fmtx(gr.hier_block2):
    def __init__(self, lo_freq, audio_rate, if_rate):

        gr.hier_block2.__init__(self, "build_fm",
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))

        fmtx = analog.nbfm_tx(audio_rate, if_rate, max_dev=5e3,
	                      tau=75e-6, fh=0.925*if_rate/2.0)

        # Local oscillator
        lo = analog.sig_source_c(if_rate,            # sample rate
                                 analog.GR_SIN_WAVE, # waveform type
                                 lo_freq,            # frequency
                                 1.0,                # amplitude
                                 0)                  # DC Offset
        mixer = blocks.multiply_cc()

        self.connect(self, fmtx, (mixer, 0))
        self.connect(lo, (mixer, 1))
        self.connect(mixer, self)

class fmtest(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self._nsamples = 1000000
        self._audio_rate = 8000

        # Set up N channels with their own baseband and IF frequencies
        self._N = 5
        chspacing = 16000
        freq = [10, 20, 30, 40, 50]
        f_lo = [0, 1*chspacing, -1*chspacing, 2*chspacing, -2*chspacing]

        self._if_rate = 4*self._N*self._audio_rate

        # Create a signal source and frequency modulate it
        self.sum = blocks.add_cc()
        for n in xrange(self._N):
            sig = analog.sig_source_f(self._audio_rate, analog.GR_SIN_WAVE, freq[n], 0.5)
            fm = fmtx(f_lo[n], self._audio_rate, self._if_rate)
            self.connect(sig, fm)
            self.connect(fm, (self.sum, n))

        self.head = blocks.head(gr.sizeof_gr_complex, self._nsamples)
        self.snk_tx = blocks.vector_sink_c()
        self.channel = channels.channel_model(0.1)

        self.connect(self.sum, self.head, self.channel, self.snk_tx)


        # Design the channlizer
        self._M = 10
        bw = chspacing/2.0
        t_bw = chspacing/10.0
        self._chan_rate = self._if_rate / self._M
        self._taps = filter.firdes.low_pass_2(1, self._if_rate, bw, t_bw,
                                              attenuation_dB=100,
                                              window=filter.firdes.WIN_BLACKMAN_hARRIS)
        tpc = math.ceil(float(len(self._taps)) /  float(self._M))

        print "Number of taps:     ", len(self._taps)
        print "Number of channels: ", self._M
        print "Taps per channel:   ", tpc

        self.pfb = filter.pfb.channelizer_ccf(self._M, self._taps)

        self.connect(self.channel, self.pfb)

        # Create a file sink for each of M output channels of the filter and connect it
        self.fmdet = list()
        self.squelch = list()
        self.snks = list()
        for i in xrange(self._M):
            self.fmdet.append(analog.nbfm_rx(self._audio_rate, self._chan_rate))
            self.squelch.append(analog.standard_squelch(self._audio_rate*10))
            self.snks.append(blocks.vector_sink_f())
            self.connect((self.pfb, i), self.fmdet[i], self.squelch[i], self.snks[i])

    def num_tx_channels(self):
        return self._N

    def num_rx_channels(self):
        return self._M

def main():

    fm = fmtest()

    tstart = time.time()
    fm.run()
    tend = time.time()

    if 1:
        fig1 = pylab.figure(1, figsize=(12,10), facecolor="w")
        fig2 = pylab.figure(2, figsize=(12,10), facecolor="w")
        fig3 = pylab.figure(3, figsize=(12,10), facecolor="w")

        Ns = 10000
        Ne = 100000

        fftlen = 8192
        winfunc = scipy.blackman

        # Plot transmitted signal
        fs = fm._if_rate

        d = fm.snk_tx.data()[Ns:Ns+Ne]
        sp1_f = fig1.add_subplot(2, 1, 1)

        X,freq = sp1_f.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs,
                           window = lambda d: d*winfunc(fftlen),
                           visible=False)
        X_in = 10.0*scipy.log10(abs(fftpack.fftshift(X)))
        f_in = scipy.arange(-fs/2.0, fs/2.0, fs/float(X_in.size))
        p1_f = sp1_f.plot(f_in, X_in, "b")
        sp1_f.set_xlim([min(f_in), max(f_in)+1])
        sp1_f.set_ylim([-120.0, 20.0])

        sp1_f.set_title("Input Signal", weight="bold")
        sp1_f.set_xlabel("Frequency (Hz)")
        sp1_f.set_ylabel("Power (dBW)")

        Ts = 1.0/fs
        Tmax = len(d)*Ts

        t_in = scipy.arange(0, Tmax, Ts)
        x_in = scipy.array(d)
        sp1_t = fig1.add_subplot(2, 1, 2)
        p1_t = sp1_t.plot(t_in, x_in.real, "b-o")
        #p1_t = sp1_t.plot(t_in, x_in.imag, "r-o")
        sp1_t.set_ylim([-5, 5])

        # Set up the number of rows and columns for plotting the subfigures
        Ncols = int(scipy.floor(scipy.sqrt(fm.num_rx_channels())))
        Nrows = int(scipy.floor(fm.num_rx_channels() / Ncols))
        if(fm.num_rx_channels() % Ncols != 0):
            Nrows += 1

        # Plot each of the channels outputs. Frequencies on Figure 2 and
        # time signals on Figure 3
        fs_o = fm._audio_rate
        for i in xrange(len(fm.snks)):
            # remove issues with the transients at the beginning
            # also remove some corruption at the end of the stream
            #    this is a bug, probably due to the corner cases
            d = fm.snks[i].data()[Ns:Ne]

            sp2_f = fig2.add_subplot(Nrows, Ncols, 1+i)
            X,freq = sp2_f.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs_o,
                               window = lambda d: d*winfunc(fftlen),
                               visible=False)
            #X_o = 10.0*scipy.log10(abs(fftpack.fftshift(X)))
            X_o = 10.0*scipy.log10(abs(X))
            #f_o = scipy.arange(-fs_o/2.0, fs_o/2.0, fs_o/float(X_o.size))
            f_o = scipy.arange(0, fs_o/2.0, fs_o/2.0/float(X_o.size))
            p2_f = sp2_f.plot(f_o, X_o, "b")
            sp2_f.set_xlim([min(f_o), max(f_o)+0.1])
            sp2_f.set_ylim([-120.0, 20.0])
            sp2_f.grid(True)

            sp2_f.set_title(("Channel %d" % i), weight="bold")
            sp2_f.set_xlabel("Frequency (kHz)")
            sp2_f.set_ylabel("Power (dBW)")


            Ts = 1.0/fs_o
            Tmax = len(d)*Ts
            t_o = scipy.arange(0, Tmax, Ts)

            x_t = scipy.array(d)
            sp2_t = fig3.add_subplot(Nrows, Ncols, 1+i)
            p2_t = sp2_t.plot(t_o, x_t.real, "b")
            p2_t = sp2_t.plot(t_o, x_t.imag, "r")
            sp2_t.set_xlim([min(t_o), max(t_o)+1])
            sp2_t.set_ylim([-1, 1])

            sp2_t.set_xlabel("Time (s)")
            sp2_t.set_ylabel("Amplitude")


        pylab.show()


if __name__ == "__main__":
    main()
