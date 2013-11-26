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
import math

import pmt
from gnuradio import gr, gr_unittest, digital, blocks, filter

def make_parabolic_pulse_shape(sps, N=1, scale=1):
    # Make L taps on each side.
    L = int(float(N)/2 * sps)
    n_taps = 2 * L + 1
    taps = []
    for tap_i in xrange(n_taps):
        x = float(tap_i - L)/sps/N
        if abs(x) > 1:
            taps.append(0)
        else:
            taps.append(1-x*x)
    return [tap*scale for tap in taps]
            
class test_correlate_and_sync(gr_unittest.TestCase):

    def test_001(self):
        # We're using a really simple preamble so that the correlation
        # is straight forward.
        preamble = [0, 0, 0, 1, 0, 0, 0]
        # Our pulse shape has this width (in units of symbols).
        pulse_width = 1.5
        # The number of filters to use for resampling.
        n_filters = 12
        sps = 3
        data = [0]*10 + preamble + [0]*40
        src = blocks.vector_source_c(data)

        # We want to generate taps with a sampling rate of sps=n_filters for resampling
        # purposes.
        pulse_shape = make_parabolic_pulse_shape(sps=n_filters, N=0.5, scale=35)

        # Create our resampling filter to generate the data for the correlator.
        shape = filter.pfb_arb_resampler_ccf(sps, pulse_shape, n_filters)
        # Generate the correlator block itself.
        correlator = digital.correlate_and_sync_cc(preamble, pulse_shape, sps, n_filters)

        # Connect it all up and go.
        snk = blocks.vector_sink_c()
        null = blocks.null_sink(gr.sizeof_gr_complex)
        tb = gr.top_block()
        tb.connect(src, shape, correlator, snk)
        tb.connect((correlator, 1), null)
        tb.run()

        # Look at the tags.  Retrieve the timing offset.
        data = snk.data()
        offset = None
        timing_error = None
        for tag in snk.tags():
            key = pmt.symbol_to_string(tag.key)
            if key == "time_est":
                offset = tag.offset
                timing_error = pmt.to_double(tag.value)
        if offset is None:
            raise ValueError("No tags found.")
        # Detect where the middle of the preamble is.
        # Assume we have only one peak and that it is symmetric.
        sum_id = 0
        sum_d = 0
        for i, d in enumerate(data):
            sum_id += i*abs(d)
            sum_d += abs(d)
        data_i = sum_id/sum_d
        if offset is not None:
            diff = data_i-offset
            remainder = -(diff%sps)
            if remainder < -sps/2.0:
                remainder += sps
        tol = 0.2
        difference = timing_error - remainder
        difference = difference % sps
        if abs(difference) >= tol:
            print("Tag gives timing estimate of {0}. QA calculates it as {1}.  Tolerance is {2}".format(timing_error, remainder, tol))
        self.assertTrue(abs(difference) < tol)


if __name__ == '__main__':
    gr_unittest.run(test_correlate_and_sync, "test_correlate_and_sync.xml")
