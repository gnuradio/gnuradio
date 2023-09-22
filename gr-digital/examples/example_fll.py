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


class example_fll(gr.top_block):
    def __init__(self, N, sps, rolloff, ntaps, bw, noise, foffset, toffset, poffset):
        gr.top_block.__init__(self)

        rrc_taps = filter.firdes.root_raised_cosine(
            sps, sps, 1.0, rolloff, ntaps)

        data = 2.0 * numpy.random.randint(0, 2, N) - 1.0
        data = numpy.exp(1j * poffset) * data

        self.src = blocks.vector_source_c(data.tolist(), False)
        self.rrc = filter.interp_fir_filter_ccf(sps, rrc_taps)
        self.chn = channels.channel_model(noise, foffset, toffset)
        self.fll = digital.fll_band_edge_cc(sps, rolloff, ntaps, bw)

        self.vsnk_src = blocks.vector_sink_c()
        self.vsnk_fll = blocks.vector_sink_c()
        self.vsnk_frq = blocks.vector_sink_f()
        self.vsnk_phs = blocks.vector_sink_f()
        self.vsnk_err = blocks.vector_sink_f()

        self.connect(self.src, self.rrc, self.chn, self.fll, self.vsnk_fll)
        self.connect(self.rrc, self.vsnk_src)
        self.connect((self.fll, 1), self.vsnk_frq)
        self.connect((self.fll, 2), self.vsnk_phs)
        self.connect((self.fll, 3), self.vsnk_err)


def main():
    parser = ArgumentParser(conflict_handler="resolve")
    parser.add_argument("-N", "--nsamples", type=int, default=2000,
                        help="Set the number of samples to process [default=%(default)r]")
    parser.add_argument("-S", "--sps", type=int, default=4,
                        help="Set the samples per symbol [default=%(default)r]")
    parser.add_argument("-r", "--rolloff", type=eng_float, default=0.35,
                        help="Set the rolloff factor [default=%(default)r]")
    parser.add_argument("-W", "--bandwidth", type=eng_float, default=2 * numpy.pi / 100.0,
                        help="Set the loop bandwidth [default=%(default)r]")
    parser.add_argument("-n", "--ntaps", type=int, default=45,
                        help="Set the number of taps in the filters [default=%(default)r]")
    parser.add_argument("--noise", type=eng_float, default=0.0,
                        help="Set the simulation noise voltage [default=%(default)r]")
    parser.add_argument("-f", "--foffset", type=eng_float, default=0.2,
                        help="Set the simulation's normalized frequency offset (in Hz) [default=%(default)r]")
    parser.add_argument("-t", "--toffset", type=eng_float, default=1.0,
                        help="Set the simulation's timing offset [default=%(default)r]")
    parser.add_argument("-p", "--poffset", type=eng_float, default=0.0,
                        help="Set the simulation's phase offset [default=%(default)r]")
    args = parser.parse_args()

    # Adjust N for the interpolation by sps
    args.nsamples = args.nsamples // args.sps

    # Set up the program-under-test
    put = example_fll(args.nsamples, args.sps, args.rolloff,
                      args.ntaps, args.bandwidth, args.noise,
                      args.foffset, args.toffset, args.poffset)
    put.run()

    data_src = numpy.array(put.vsnk_src.data())
    data_err = numpy.array(put.vsnk_err.data())

    # Convert the FLL's LO frequency from rads/sec to Hz
    data_frq = numpy.array(put.vsnk_frq.data()) / (2.0 * numpy.pi)

    # adjust this to align with the data. There are 2 filters of
    # ntaps long and the channel introduces another 4 sample delay.
    data_fll = numpy.array(put.vsnk_fll.data()[2 * args.ntaps - 4:])

    # Plot the FLL's LO frequency
    f1 = pyplot.figure(1, figsize=(12, 10))
    s1 = f1.add_subplot(2, 2, 1)
    s1.plot(data_frq)
    s1.set_title("FLL LO")
    s1.set_xlabel("Samples")
    s1.set_ylabel("Frequency (normalized Hz)")

    # Plot the FLL's error
    s2 = f1.add_subplot(2, 2, 2)
    s2.plot(data_err)
    s2.set_title("FLL Error")
    s2.set_xlabel("Samples")
    s2.set_ylabel("FLL Loop error")

    # Plot the IQ symbols
    s3 = f1.add_subplot(2, 2, 3)
    s3.plot(data_src.real, data_src.imag, "o")
    s3.plot(data_fll.real, data_fll.imag, "rx")
    s3.set_title("IQ")
    s3.set_xlabel("Real part")
    s3.set_ylabel("Imag part")

    # Plot the symbols in time
    s4 = f1.add_subplot(2, 2, 4)
    s4.plot(data_src.real, "o-")
    s4.plot(data_fll.real, "rx-")
    s4.set_title("Symbols")
    s4.set_xlabel("Samples")
    s4.set_ylabel("Real Part of Signals")

    pyplot.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
