#!/usr/bin/env python
#
# Copyright 2004,2010,2012 Free Software Foundation, Inc.
# Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr_unittest, trellis
from gnuradio.gr import major_version, api_version

GR_VERSION = (int(major_version()), int(api_version()))


class test_interleaver(gr_unittest.TestCase):

    def test_001_explicit_interleaver_vector_with_length(self):
        if GR_VERSION >= (3, 11):
            self.skipTest("skipping test of removed constructor")
        # deprecated, since length inherent to vector
        k = 5
        interl = [1, 2, 3, 4, 0]
        de_in = [4, 0, 1, 2, 3]
        i = trellis.interleaver(k, interl)
        self.assertEqual((k, interl, de_in), (i.k(), i.interleaver_indices(), i.deinterleaver_indices()))

    def test_002_explicit_interleaver_vector(self):
        interl = [1, 2, 3, 4, 0]
        de_in = [4, 0, 1, 2, 3]
        i = trellis.interleaver(interl)
        self.assertEqual((interl, de_in), (i.interleaver_indices(), i.deinterleaver_indices()))

    def test_002_random(self):
        k = 1023
        seed = 42
        i = trellis.interleaver(k, seed)
        self.assertSequenceEqual(sorted(i.interleaver_indices()), range(k))
        decorated_indices = [
            (interleave_idx, idx) for idx, interleave_idx in enumerate(i.interleaver_indices())
        ]
        reordered = sorted(decorated_indices)
        deinterleaver = [tup[1] for tup in reordered]
        self.assertSequenceEqual(deinterleaver, i.deinterleaver_indices())


if __name__ == "__main__":
    gr_unittest.run(test_interleaver)
