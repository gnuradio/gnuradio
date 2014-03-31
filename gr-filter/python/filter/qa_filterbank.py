#!/usr/bin/env python
#
# Copyright 2012,2014 Free Software Foundation, Inc.
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

import time
import random
import math

from gnuradio import gr, gr_unittest, filter, blocks

def convolution(A, B):
    """
    Returns a convolution of the A and B vectors of length
    len(A)-len(B).
    """
    rs = []
    for i in range(len(B)-1, len(A)):
        r = 0
        for j, b in enumerate(B):
            r += A[i-j] * b
        rs.append(r)
    return rs

class test_filterbank_vcvcf(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        """
        Generates nfilts sets of random complex data.
        Generates two sets of random taps for each filter.
        Applies one set of the random taps, gets some output,
        applies the second set of random taps, gets some more output,
        The output is then compared with a python-implemented
        convolution.
        """
        myrand = random.Random(123).random
        nfilts = 10
        ntaps = 5
        # Sets some of the taps to be all zeros.
        zero_filts1 = (3, 7)
        zero_filts2 = (1, 6, 9)
        ndatapoints = 100
        # Generate some random sets of data
        data_sets = []
        for i in range(0, nfilts):
            data_sets.append([(myrand()-0.5) + (myrand()-0.5)*(0+1j)
                              for k in range(0, ndatapoints)])
        # Join them together to pass to vector_source block
        data = []
        for dp in zip(*data_sets):
            data += dp
        # Generate some random taps.
        taps1 = []
        taps2 = []
        for i in range(0, nfilts):
            if i in zero_filts1:
                taps1.append([0]*ntaps)
            else:
                taps1.append([myrand()-0.5 for k in range(0, ntaps)])
            if i in zero_filts2:
                taps2.append([0]*ntaps)
            else:
                taps2.append([myrand()-0.5 for k in range(0, ntaps)])

        # Calculate results with a python-implemented convolution.
        results = []
        results2 = []
        for ds, ts, ts2 in zip(data_sets, taps1, taps2):
            results.append(convolution(ds[-len(ts):]+ds[:-1], ts))
            results2.append(convolution(ds[-len(ts):]+ds[:-1], ts2))
        # Convert results from 2D arrays to 1D arrays for ease of comparison.
        comb_results = []
        for rs in zip(*results):
            comb_results += rs
        comb_results2 = []
        for rs in zip(*results2):
            comb_results2 += rs
        # Construct the signal-processing chain.
        src = blocks.vector_source_c(data, True, nfilts)
        fb = filter.filterbank_vcvcf(taps1)
        v2s = blocks.vector_to_stream(gr.sizeof_gr_complex, nfilts)
        s2v = blocks.stream_to_vector(gr.sizeof_gr_complex, nfilts*ndatapoints)
        snk = blocks.probe_signal_vc(nfilts*ndatapoints)
        self.tb.connect(src, fb, v2s, s2v, snk)
        # Run the signal-processing chain.
        self.tb.start()
        all_zero = True
        outdata = None
        waittime = 0.001
        # Wait until we have some data.
        while (not outdata) or outdata[0]==0:
            time.sleep(waittime)
            outdata = snk.level()
        # Apply the second set of taps.
        fb.set_taps(taps2)
        outdata2 = None
        # Wait until we have new data.
        while (not outdata2) or outdata[0] == outdata2[0]:
            time.sleep(waittime)
            outdata2 = snk.level()
        self.tb.stop()
        # Compare the datasets.
        self.assertComplexTuplesAlmostEqual(comb_results, outdata, 6)
        self.assertComplexTuplesAlmostEqual(comb_results2, outdata2, 6)

if __name__ == '__main__':
    gr_unittest.run(test_filterbank_vcvcf, "test_filterbank_vcvcf.xml")
