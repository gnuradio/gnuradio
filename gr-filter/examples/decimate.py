#!/usr/bin/env python
#
# Copyright 2009,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter
from gnuradio.fft import window
import sys, time
import numpy

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)


try:
    from matplotlib import pyplot
    from matplotlib import pyplot as mlab
except ImportError:
    sys.stderr.write("Error: Program requires matplotlib (see: matplotlib.sourceforge.net).\n")
    sys.exit(1)

class pfb_top_block(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        self._N = 10000000      # number of samples to use
        self._fs = 10000        # initial sampling rate
        self._decim = 20        # Decimation rate

        # Generate the prototype filter taps for the decimators with a 200 Hz bandwidth
        self._taps = filter.firdes.low_pass_2(1, self._fs,
                                              200, 150,
                                              attenuation_dB=120,
                                              window=window.WIN_BLACKMAN_hARRIS)

        # Calculate the number of taps per channel for our own information
        tpc = numpy.ceil(float(len(self._taps)) / float(self._decim))
        print("Number of taps:     ", len(self._taps))
        print("Number of filters:  ", self._decim)
        print("Taps per channel:   ", tpc)

        # Build the input signal source
        # We create a list of freqs, and a sine wave is generated and added to the source
        # for each one of these frequencies.
        self.signals = list()
        self.add = blocks.add_cc()
        freqs = [10, 20, 2040]
        for i in range(len(freqs)):
            self.signals.append(analog.sig_source_c(self._fs, analog.GR_SIN_WAVE, freqs[i], 1))
            self.connect(self.signals[i], (self.add,i))

        self.head = blocks.head(gr.sizeof_gr_complex, self._N)

        # Construct a PFB decimator filter
        self.pfb = filter.pfb.decimator_ccf(self._decim, self._taps, 0)

        # Construct a standard FIR decimating filter
        self.dec = filter.fir_filter_ccf(self._decim, self._taps)

        self.snk_i = blocks.vector_sink_c()

        # Connect the blocks
        self.connect(self.add, self.head, self.pfb)
        self.connect(self.add, self.snk_i)

        # Create the sink for the decimated siganl
        self.snk = blocks.vector_sink_c()
        self.connect(self.pfb, self.snk)


def main():
    tb = pfb_top_block()

    tstart = time.time()
    tb.run()
    tend = time.time()
    print("Run time: %f" % (tend - tstart))

    if 1:
        fig1 = pyplot.figure(1, figsize=(16,9))
        fig2 = pyplot.figure(2, figsize=(16,9))

        Ns = 10000
        Ne = 10000

        fftlen = 8192
        winfunc = numpy.blackman
        fs = tb._fs

        # Plot the input to the decimator

        d = tb.snk_i.data()[Ns:Ns+Ne]
        sp1_f = fig1.add_subplot(2, 1, 1)

        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen / 4, Fs=fs,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_in = 10.0*numpy.log10(abs(numpy.fft.fftshift(X)))
        f_in = numpy.arange(-fs / 2.0, fs / 2.0, fs / float(X_in.size))
        p1_f = sp1_f.plot(f_in, X_in, "b")
        sp1_f.set_xlim([min(f_in), max(f_in)+1])
        sp1_f.set_ylim([-200.0, 50.0])

        sp1_f.set_title("Input Signal", weight="bold")
        sp1_f.set_xlabel("Frequency (Hz)")
        sp1_f.set_ylabel("Power (dBW)")

        Ts = 1.0 / fs
        Tmax = len(d)*Ts

        t_in = numpy.arange(0, Tmax, Ts)
        x_in = numpy.array(d)
        sp1_t = fig1.add_subplot(2, 1, 2)
        p1_t = sp1_t.plot(t_in, x_in.real, "b")
        p1_t = sp1_t.plot(t_in, x_in.imag, "r")
        sp1_t.set_ylim([-tb._decim*1.1, tb._decim*1.1])

        sp1_t.set_xlabel("Time (s)")
        sp1_t.set_ylabel("Amplitude")


        # Plot the output of the decimator
        fs_o = tb._fs / tb._decim

        sp2_f = fig2.add_subplot(2, 1, 1)
        d = tb.snk.data()[Ns:Ns+Ne]
        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen / 4, Fs=fs_o,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_o = 10.0*numpy.log10(abs(numpy.fft.fftshift(X)))
        f_o = numpy.arange(-fs_o / 2.0, fs_o / 2.0, fs_o / float(X_o.size))
        p2_f = sp2_f.plot(f_o, X_o, "b")
        sp2_f.set_xlim([min(f_o), max(f_o)+1])
        sp2_f.set_ylim([-200.0, 50.0])

        sp2_f.set_title("PFB Decimated Signal", weight="bold")
        sp2_f.set_xlabel("Frequency (Hz)")
        sp2_f.set_ylabel("Power (dBW)")


        Ts_o = 1.0 / fs_o
        Tmax_o = len(d)*Ts_o

        x_o = numpy.array(d)
        t_o = numpy.arange(0, Tmax_o, Ts_o)
        sp2_t = fig2.add_subplot(2, 1, 2)
        p2_t = sp2_t.plot(t_o, x_o.real, "b-o")
        p2_t = sp2_t.plot(t_o, x_o.imag, "r-o")
        sp2_t.set_ylim([-2.5, 2.5])

        sp2_t.set_xlabel("Time (s)")
        sp2_t.set_ylabel("Amplitude")

        pyplot.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

