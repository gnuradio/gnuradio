#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
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

from gnuradio import gr, digital, filter
from gnuradio import blocks
from gnuradio import channels
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

class example_costas(gr.top_block):
    def __init__(self, N, sps, rolloff, ntaps, bw, noise, foffset, toffset, poffset):
        gr.top_block.__init__(self)

        rrc_taps = filter.firdes.root_raised_cosine(
            sps, sps, 1.0, rolloff, ntaps)

        data = 2.0*scipy.random.randint(0, 2, N) - 1.0
        data = scipy.exp(1j*poffset) * data

        self.src = blocks.vector_source_c(data.tolist(), False)
        self.rrc = filter.interp_fir_filter_ccf(sps, rrc_taps)
        self.chn = channels.channel_model(noise, foffset, toffset)
        self.cst = digital.costas_loop_cc(bw, 2)

        self.vsnk_src = blocks.vector_sink_c()
        self.vsnk_cst = blocks.vector_sink_c()
        self.vsnk_frq = blocks.vector_sink_f()

        self.connect(self.src, self.rrc, self.chn, self.cst, self.vsnk_cst)
        self.connect(self.rrc, self.vsnk_src)
        self.connect((self.cst,1), self.vsnk_frq)

def main():
    parser = OptionParser(option_class=eng_option, conflict_handler="resolve")
    parser.add_option("-N", "--nsamples", type="int", default=2000,
                      help="Set the number of samples to process [default=%default]")
    parser.add_option("-S", "--sps", type="int", default=4,
                      help="Set the samples per symbol [default=%default]")
    parser.add_option("-r", "--rolloff", type="eng_float", default=0.35,
                      help="Set the rolloff factor [default=%default]")
    parser.add_option("-W", "--bandwidth", type="eng_float", default=2*scipy.pi/100.0,
                      help="Set the loop bandwidth [default=%default]")
    parser.add_option("-n", "--ntaps", type="int", default=45,
                      help="Set the number of taps in the filters [default=%default]")
    parser.add_option("", "--noise", type="eng_float", default=0.0,
                      help="Set the simulation noise voltage [default=%default]")
    parser.add_option("-f", "--foffset", type="eng_float", default=0.0,
                      help="Set the simulation's normalized frequency offset (in Hz) [default=%default]")
    parser.add_option("-t", "--toffset", type="eng_float", default=1.0,
                      help="Set the simulation's timing offset [default=%default]")
    parser.add_option("-p", "--poffset", type="eng_float", default=0.707,
                      help="Set the simulation's phase offset [default=%default]")
    (options, args) = parser.parse_args ()

    # Adjust N for the interpolation by sps
    options.nsamples = options.nsamples // options.sps

    # Set up the program-under-test
    put = example_costas(options.nsamples, options.sps, options.rolloff,
                         options.ntaps, options.bandwidth, options.noise,
                         options.foffset, options.toffset, options.poffset)
    put.run()

    data_src = scipy.array(put.vsnk_src.data())

    # Convert the FLL's LO frequency from rads/sec to Hz
    data_frq = scipy.array(put.vsnk_frq.data()) / (2.0*scipy.pi)

    # adjust this to align with the data.
    data_cst = scipy.array(3*[0,]+list(put.vsnk_cst.data()))

    # Plot the Costas loop's LO frequency
    f1 = pylab.figure(1, figsize=(12,10), facecolor='w')
    s1 = f1.add_subplot(2,2,1)
    s1.plot(data_frq)
    s1.set_title("Costas LO")
    s1.set_xlabel("Samples")
    s1.set_ylabel("Frequency (normalized Hz)")

    # Plot the IQ symbols
    s3 = f1.add_subplot(2,2,2)
    s3.plot(data_src.real, data_src.imag, "o")
    s3.plot(data_cst.real, data_cst.imag, "rx")
    s3.set_title("IQ")
    s3.set_xlabel("Real part")
    s3.set_ylabel("Imag part")
    s3.set_xlim([-2, 2])
    s3.set_ylim([-2, 2])

    # Plot the symbols in time
    s4 = f1.add_subplot(2,2,3)
    s4.set_position([0.125, 0.05, 0.775, 0.4])
    s4.plot(data_src.real, "o-")
    s4.plot(data_cst.real, "rx-")
    s4.set_title("Symbols")
    s4.set_xlabel("Samples")
    s4.set_ylabel("Real Part of Signals")

    pylab.show()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass

