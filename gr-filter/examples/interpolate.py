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

        self._N = 100000        # number of samples to use
        self._fs = 2000         # initial sampling rate
        self._interp = 5        # Interpolation rate for PFB interpolator
        self._ainterp = 5.5       # Resampling rate for the PFB arbitrary resampler

        # Frequencies of the signals we construct
        freq1 = 100
        freq2 = 200

        # Create a set of taps for the PFB interpolator
        # This is based on the post-interpolation sample rate
        self._taps = filter.firdes.low_pass_2(self._interp,
                                              self._interp*self._fs,
                                              freq2+50, 50,
                                              attenuation_dB=120,
                                              window=filter.firdes.WIN_BLACKMAN_hARRIS)

        # Create a set of taps for the PFB arbitrary resampler
        # The filter size is the number of filters in the filterbank; 32 will give very low side-lobes,
        # and larger numbers will reduce these even farther
        # The taps in this filter are based on a sampling rate of the filter size since it acts
        # internally as an interpolator.
        flt_size = 32
        self._taps2 = filter.firdes.low_pass_2(flt_size,
                                               flt_size*self._fs,
                                               freq2+50, 150,
                                               attenuation_dB=120,
                                               window=filter.firdes.WIN_BLACKMAN_hARRIS)

        # Calculate the number of taps per channel for our own information
        tpc = scipy.ceil(float(len(self._taps)) /  float(self._interp))
        print "Number of taps:     ", len(self._taps)
        print "Number of filters:  ", self._interp
        print "Taps per channel:   ", tpc

        # Create a couple of signals at different frequencies
        self.signal1 = analog.sig_source_c(self._fs, analog.GR_SIN_WAVE, freq1, 0.5)
        self.signal2 = analog.sig_source_c(self._fs, analog.GR_SIN_WAVE, freq2, 0.5)
        self.signal = blocks.add_cc()

        self.head = blocks.head(gr.sizeof_gr_complex, self._N)

        # Construct the PFB interpolator filter
        self.pfb = filter.pfb.interpolator_ccf(self._interp, self._taps)

        # Construct the PFB arbitrary resampler filter
        self.pfb_ar = filter.pfb.arb_resampler_ccf(self._ainterp, self._taps2, flt_size)
        self.snk_i = blocks.vector_sink_c()

        #self.pfb_ar.pfb.print_taps()
        #self.pfb.pfb.print_taps()

        # Connect the blocks
        self.connect(self.signal1, self.head, (self.signal,0))
        self.connect(self.signal2, (self.signal,1))
        self.connect(self.signal, self.pfb)
        self.connect(self.signal, self.pfb_ar)
        self.connect(self.signal, self.snk_i)

        # Create the sink for the interpolated signals
        self.snk1 = blocks.vector_sink_c()
        self.snk2 = blocks.vector_sink_c()
        self.connect(self.pfb, self.snk1)
        self.connect(self.pfb_ar, self.snk2)


def main():
    tb = pfb_top_block()

    tstart = time.time()
    tb.run()
    tend = time.time()
    print "Run time: %f" % (tend - tstart)


    if 1:
        fig1 = pylab.figure(1, figsize=(12,10), facecolor="w")
        fig2 = pylab.figure(2, figsize=(12,10), facecolor="w")
        fig3 = pylab.figure(3, figsize=(12,10), facecolor="w")

        Ns = 10000
        Ne = 10000

        fftlen = 8192
        winfunc = scipy.blackman

        # Plot input signal
        fs = tb._fs

        d = tb.snk_i.data()[Ns:Ns+Ne]
        sp1_f = fig1.add_subplot(2, 1, 1)

        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_in = 10.0*scipy.log10(abs(fftpack.fftshift(X)))
        f_in = scipy.arange(-fs/2.0, fs/2.0, fs/float(X_in.size))
        p1_f = sp1_f.plot(f_in, X_in, "b")
        sp1_f.set_xlim([min(f_in), max(f_in)+1])
        sp1_f.set_ylim([-200.0, 50.0])


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
        sp1_t.set_ylim([-2.5, 2.5])

        sp1_t.set_title("Input Signal", weight="bold")
        sp1_t.set_xlabel("Time (s)")
        sp1_t.set_ylabel("Amplitude")


        # Plot output of PFB interpolator
        fs_int = tb._fs*tb._interp

        sp2_f = fig2.add_subplot(2, 1, 1)
        d = tb.snk1.data()[Ns:Ns+(tb._interp*Ne)]
        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_o = 10.0*scipy.log10(abs(fftpack.fftshift(X)))
        f_o = scipy.arange(-fs_int/2.0, fs_int/2.0, fs_int/float(X_o.size))
        p2_f = sp2_f.plot(f_o, X_o, "b")
        sp2_f.set_xlim([min(f_o), max(f_o)+1])
        sp2_f.set_ylim([-200.0, 50.0])

        sp2_f.set_title("Output Signal from PFB Interpolator", weight="bold")
        sp2_f.set_xlabel("Frequency (Hz)")
        sp2_f.set_ylabel("Power (dBW)")

        Ts_int = 1.0/fs_int
        Tmax = len(d)*Ts_int

        t_o = scipy.arange(0, Tmax, Ts_int)
        x_o1 = scipy.array(d)
        sp2_t = fig2.add_subplot(2, 1, 2)
        p2_t = sp2_t.plot(t_o, x_o1.real, "b-o")
        #p2_t = sp2_t.plot(t_o, x_o.imag, "r-o")
        sp2_t.set_ylim([-2.5, 2.5])

        sp2_t.set_title("Output Signal from PFB Interpolator", weight="bold")
        sp2_t.set_xlabel("Time (s)")
        sp2_t.set_ylabel("Amplitude")


        # Plot output of PFB arbitrary resampler
        fs_aint = tb._fs * tb._ainterp

        sp3_f = fig3.add_subplot(2, 1, 1)
        d = tb.snk2.data()[Ns:Ns+(tb._interp*Ne)]
        X,freq = mlab.psd(d, NFFT=fftlen, noverlap=fftlen/4, Fs=fs,
                          window = lambda d: d*winfunc(fftlen),
                          scale_by_freq=True)
        X_o = 10.0*scipy.log10(abs(fftpack.fftshift(X)))
        f_o = scipy.arange(-fs_aint/2.0, fs_aint/2.0, fs_aint/float(X_o.size))
        p3_f = sp3_f.plot(f_o, X_o, "b")
        sp3_f.set_xlim([min(f_o), max(f_o)+1])
        sp3_f.set_ylim([-200.0, 50.0])

        sp3_f.set_title("Output Signal from PFB Arbitrary Resampler", weight="bold")
        sp3_f.set_xlabel("Frequency (Hz)")
        sp3_f.set_ylabel("Power (dBW)")

        Ts_aint = 1.0/fs_aint
        Tmax = len(d)*Ts_aint

        t_o = scipy.arange(0, Tmax, Ts_aint)
        x_o2 = scipy.array(d)
        sp3_f = fig3.add_subplot(2, 1, 2)
        p3_f = sp3_f.plot(t_o, x_o2.real, "b-o")
        p3_f = sp3_f.plot(t_o, x_o1.real, "m-o")
        #p3_f = sp3_f.plot(t_o, x_o2.imag, "r-o")
        sp3_f.set_ylim([-2.5, 2.5])

        sp3_f.set_title("Output Signal from PFB Arbitrary Resampler", weight="bold")
        sp3_f.set_xlabel("Time (s)")
        sp3_f.set_ylabel("Amplitude")

        pylab.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

