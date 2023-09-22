#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, digital, filter
from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser
import sys
import numpy

try:
    from matplotlib import pyplot
except ImportError:
    print("Error: could not from matplotlib import pyplot (http://matplotlib.sourceforge.net/)")
    sys.exit(1)


class example_timing(gr.top_block):
    def __init__(self, N, sps, rolloff, ntaps, bw, noise,
                 foffset, toffset, poffset, mode=0):
        gr.top_block.__init__(self)

        rrc_taps = filter.firdes.root_raised_cosine(
            sps, sps, 1.0, rolloff, ntaps)

        gain = bw
        nfilts = 32
        rrc_taps_rx = filter.firdes.root_raised_cosine(
            nfilts, sps * nfilts, 1.0, rolloff, ntaps * nfilts)

        data = 2.0 * numpy.random.randint(0, 2, N) - 1.0
        data = numpy.exp(1j * poffset) * data

        self.src = blocks.vector_source_c(data.tolist(), False)
        self.rrc = filter.interp_fir_filter_ccf(sps, rrc_taps)
        self.chn = channels.channel_model(noise, foffset, toffset)
        self.off = filter.mmse_resampler_cc(0.20, 1.0)

        if mode == 0:
            self.clk = digital.pfb_clock_sync_ccf(sps, gain, rrc_taps_rx,
                                                  nfilts, nfilts // 2, 1)
            self.taps = self.clk.taps()
            self.dtaps = self.clk.diff_taps()

            self.delay = int(numpy.ceil(((len(rrc_taps) - 1) // 2 +
                                         (len(self.taps[0]) - 1) // 2) // float(sps))) + 1

            self.vsnk_err = blocks.vector_sink_f()
            self.vsnk_rat = blocks.vector_sink_f()
            self.vsnk_phs = blocks.vector_sink_f()

            self.connect((self.clk, 1), self.vsnk_err)
            self.connect((self.clk, 2), self.vsnk_rat)
            self.connect((self.clk, 3), self.vsnk_phs)

        else:  # mode == 1
            mu = 0.5
            gain_mu = bw
            gain_omega = 0.25 * gain_mu * gain_mu
            omega_rel_lim = 0.02
            self.clk = digital.clock_recovery_mm_cc(sps, gain_omega,
                                                    mu, gain_mu,
                                                    omega_rel_lim)

            self.vsnk_err = blocks.vector_sink_f()

            self.connect((self.clk, 1), self.vsnk_err)

        self.vsnk_src = blocks.vector_sink_c()
        self.vsnk_clk = blocks.vector_sink_c()

        self.connect(self.src, self.rrc, self.chn,
                     self.off, self.clk, self.vsnk_clk)
        self.connect(self.src, self.vsnk_src)


def main():
    parser = ArgumentParser(conflict_handler="resolve")
    parser.add_argument("-N", "--nsamples", type=int, default=2000,
                        help="Set the number of samples to process [default=%(default)r]")
    parser.add_argument("-S", "--sps", type=int, default=4,
                        help="Set the samples per symbol [default=%(default)r]")
    parser.add_argument("-r", "--rolloff", type=eng_float, default=0.35,
                        help="Set the rolloff factor [default=%(default)r]")
    parser.add_argument("-W", "--bandwidth", type=eng_float, default=2 * numpy.pi / 100.0,
                        help="Set the loop bandwidth (PFB) or gain (M&M) [default=%(default)r]")
    parser.add_argument("-n", "--ntaps", type=int, default=45,
                        help="Set the number of taps in the filters [default=%(default)r]")
    parser.add_argument("--noise", type=eng_float, default=0.0,
                        help="Set the simulation noise voltage [default=%(default)r]")
    parser.add_argument("-f", "--foffset", type=eng_float, default=0.0,
                        help="Set the simulation's normalized frequency offset (in Hz) [default=%(default)r]")
    parser.add_argument("-t", "--toffset", type=eng_float, default=1.0,
                        help="Set the simulation's timing offset [default=%(default)r]")
    parser.add_argument("-p", "--poffset", type=eng_float, default=0.0,
                        help="Set the simulation's phase offset [default=%(default)r]")
    parser.add_argument("-M", "--mode", type=int, default=0,
                        help="Set the recovery mode (0: polyphase, 1: M&M) [default=%(default)r]")
    args = parser.parse_args()

    # Adjust N for the interpolation by sps
    args.nsamples = args.nsamples // args.sps

    # Set up the program-under-test
    put = example_timing(args.nsamples, args.sps, args.rolloff,
                         args.ntaps, args.bandwidth, args.noise,
                         args.foffset, args.toffset, args.poffset,
                         args.mode)
    put.run()

    if args.mode == 0:
        data_src = numpy.array(put.vsnk_src.data()[20:])
        data_clk = numpy.array(put.vsnk_clk.data()[20:])

        data_err = numpy.array(put.vsnk_err.data()[20:])
        data_rat = numpy.array(put.vsnk_rat.data()[20:])
        data_phs = numpy.array(put.vsnk_phs.data()[20:])

        f1 = pyplot.figure(1, figsize=(12, 10), facecolor='w')

        # Plot the IQ symbols
        s1 = f1.add_subplot(2, 2, 1)
        s1.plot(data_src.real, data_src.imag, "bo")
        s1.plot(data_clk.real, data_clk.imag, "ro")
        s1.set_title("IQ")
        s1.set_xlabel("Real part")
        s1.set_ylabel("Imag part")
        s1.set_xlim([-2, 2])
        s1.set_ylim([-2, 2])

        # Plot the symbols in time
        delay = put.delay
        m = len(data_clk.real)
        s2 = f1.add_subplot(2, 2, 2)
        s2.plot(data_src.real, "bs", markersize=10, label="Input")
        s2.plot(data_clk.real[delay:], "ro", label="Recovered")
        s2.set_title("Symbols")
        s2.set_xlabel("Samples")
        s2.set_ylabel("Real Part of Signals")
        s2.legend()

        # Plot the clock recovery loop's error
        s3 = f1.add_subplot(2, 2, 3)
        s3.plot(data_err, label="Error")
        s3.plot(data_rat, 'r', label="Update rate")
        s3.set_title("Clock Recovery Loop Error")
        s3.set_xlabel("Samples")
        s3.set_ylabel("Error")
        s3.set_ylim([-0.5, 0.5])
        s3.legend()

        # Plot the clock recovery loop's error
        s4 = f1.add_subplot(2, 2, 4)
        s4.plot(data_phs)
        s4.set_title("Clock Recovery Loop Filter Phase")
        s4.set_xlabel("Samples")
        s4.set_ylabel("Filter Phase")

        diff_taps = put.dtaps
        ntaps = len(diff_taps[0])
        nfilts = len(diff_taps)
        t = numpy.arange(0, ntaps * nfilts)

        f3 = pyplot.figure(3, figsize=(12, 10), facecolor='w')
        s31 = f3.add_subplot(2, 1, 1)
        s32 = f3.add_subplot(2, 1, 2)
        s31.set_title("Differential Filters")
        s32.set_title("FFT of Differential Filters")

        for i, d in enumerate(diff_taps):
            D = 20.0 * \
                numpy.log10(
                    1e-20 + abs(numpy.fft.fftshift(numpy.fft.fft(d, 10000))))
            s31.plot(t[i::nfilts].real, d, "-o")
            s32.plot(D)
        s32.set_ylim([-120, 10])

    # If testing the M&M clock recovery loop
    else:
        data_src = numpy.array(put.vsnk_src.data()[20:])
        data_clk = numpy.array(put.vsnk_clk.data()[20:])

        data_err = numpy.array(put.vsnk_err.data()[20:])

        f1 = pyplot.figure(1, figsize=(12, 10), facecolor='w')

        # Plot the IQ symbols
        s1 = f1.add_subplot(2, 2, 1)
        s1.plot(data_src.real, data_src.imag, "o")
        s1.plot(data_clk.real, data_clk.imag, "ro")
        s1.set_title("IQ")
        s1.set_xlabel("Real part")
        s1.set_ylabel("Imag part")
        s1.set_xlim([-2, 2])
        s1.set_ylim([-2, 2])

        # Plot the symbols in time
        s2 = f1.add_subplot(2, 2, 2)
        s2.plot(data_src.real, "bs", markersize=10, label="Input")
        s2.plot(data_clk.real, "ro", label="Recovered")
        s2.set_title("Symbols")
        s2.set_xlabel("Samples")
        s2.set_ylabel("Real Part of Signals")
        s2.legend()

        # Plot the clock recovery loop's error
        s3 = f1.add_subplot(2, 2, 3)
        s3.plot(data_err)
        s3.set_title("Clock Recovery Loop Error")
        s3.set_xlabel("Samples")
        s3.set_ylabel("Error")

    pyplot.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
