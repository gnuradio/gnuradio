#!/usr/bin/env python
#
# Copyright 2007,2010,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog

class test_noise_source(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        # Just confirm that we can instantiate a noise source
        op = analog.noise_source_f(analog.GR_GAUSSIAN, 10, 10)

    def test_002(self):
        # Test get methods
        set_type = analog.GR_GAUSSIAN
        set_ampl = 10
        op = analog.noise_source_f(set_type, set_ampl, 10)
        get_type = op.type()
        get_ampl = op.amplitude()

        self.assertEqual(get_type, set_type)
        self.assertEqual(get_ampl, set_ampl)


if __name__ == '__main__':
    gr_unittest.run(test_noise_source, "test_noise_source.xml")

