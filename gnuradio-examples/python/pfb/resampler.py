#!/usr/bin/env python
#
# Copyright 2009 Free Software Foundation, Inc.
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

from gnuradio import gr, blks2
import sys

try:
    import scipy
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)

try:
    import pylab
except ImportError:
    print "Error: Program requires matplotlib (see: matplotlib.sourceforge.net)."
    sys.exit(1)

class mytb(gr.top_block):
    def __init__(self, fs_in, fs_out, fc, N=10000):
        gr.top_block.__init__(self)
        
        rerate = float(fs_out) / float(fs_in)
        print "Resampling from %f to %f by %f " %(fs_in, fs_out, rerate)

        # Creating our own taps
        taps = gr.firdes.low_pass_2(32, 32, 0.25, 0.1, 80)

        self.src = gr.sig_source_c(fs_in, gr.GR_SIN_WAVE, fc, 1)
        #self.src = gr.noise_source_c(gr.GR_GAUSSIAN, 1)
        self.head = gr.head(gr.sizeof_gr_complex, N)

        # A resampler with our taps
        self.resamp_0 = blks2.pfb_arb_resampler_ccf(rerate, taps,
                                                    flt_size=32)

        # A resampler that just needs a resampling rate.
        # Filter is created for us and designed to cover
        # entire bandwidth of the input signal.
        # An optional atten=XX rate can be used here to 
        # specify the out-of-band rejection (default=80).
        self.resamp_1 = blks2.pfb_arb_resampler_ccf(rerate)

        self.snk_in = gr.vector_sink_c()
        self.snk_0 = gr.vector_sink_c()
        self.snk_1 = gr.vector_sink_c()

        self.connect(self.src, self.head, self.snk_in)
        self.connect(self.head, self.resamp_0, self.snk_0)
        self.connect(self.head, self.resamp_1, self.snk_1)

def main():
    fs_in = 8000
    fs_out = 20000
    fc = 1000
    N = 10000

    tb = mytb(fs_in, fs_out, fc, N)
    tb.run()


    # Plot PSD of signals
    nfftsize = 2048
    fig1 = pylab.figure(1, figsize=(10,10), facecolor="w")
    sp1 = fig1.add_subplot(2,1,1)
    sp1.psd(tb.snk_in.data(), NFFT=nfftsize,
            noverlap=nfftsize/4, Fs = fs_in)
    sp1.set_title(("Input Signal at f_s=%.2f kHz" % (fs_in/1000.0)))
    sp1.set_xlim([-fs_in/2, fs_in/2])

    sp2 = fig1.add_subplot(2,1,2)
    sp2.psd(tb.snk_0.data(), NFFT=nfftsize,
            noverlap=nfftsize/4, Fs = fs_out,
            label="With our filter")
    sp2.psd(tb.snk_1.data(), NFFT=nfftsize,
            noverlap=nfftsize/4, Fs = fs_out,
            label="With auto-generated filter")
    sp2.set_title(("Output Signals at f_s=%.2f kHz" % (fs_out/1000.0)))
    sp2.set_xlim([-fs_out/2, fs_out/2])
    sp2.legend()

    # Plot signals in time
    Ts_in = 1.0/fs_in
    Ts_out = 1.0/fs_out
    t_in = scipy.arange(0, len(tb.snk_in.data())*Ts_in, Ts_in)
    t_out = scipy.arange(0, len(tb.snk_0.data())*Ts_out, Ts_out)

    fig2 = pylab.figure(2, figsize=(10,10), facecolor="w")
    sp21 = fig2.add_subplot(2,1,1)
    sp21.plot(t_in, tb.snk_in.data())
    sp21.set_title(("Input Signal at f_s=%.2f kHz" % (fs_in/1000.0)))
    sp21.set_xlim([t_in[100], t_in[200]])

    sp22 = fig2.add_subplot(2,1,2)
    sp22.plot(t_out, tb.snk_0.data(),
              label="With our filter")
    sp22.plot(t_out, tb.snk_1.data(),
              label="With auto-generated filter")
    sp22.set_title(("Output Signals at f_s=%.2f kHz" % (fs_out/1000.0)))
    r = float(fs_out)/float(fs_in)
    sp22.set_xlim([t_out[r * 100], t_out[r * 200]])
    sp22.legend()

    pylab.show()

if __name__ == "__main__":
    main()

