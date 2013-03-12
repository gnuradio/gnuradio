#!/usr/bin/env python
#
# Copyright 2007,2010,2012 Free Software Foundation, Inc.
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

