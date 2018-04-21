#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks
from file_taps_loader import file_taps_loader
import inspect, os

class qa_file_taps_loader (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_simple_check_taps (self):
        # Parameters
        path = "test_taps"
        # For safety reasons work with absolute path here.
        c_path = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) + "/" + path
        verbose = False
        ftl = blocks.file_taps_loader(c_path, verbose)
        expected_taps = [ -0.01015755, -0.01920645, 0.01674679, 0.2039666, 0.49091557, 0.63547009,
                          0.49091557, 0.2039666, 0.01674679, -0.01920645, -0.01015755]
        self.assertFloatTuplesAlmostEqual(tuple(expected_taps), tuple(ftl.get_taps()), 6)

    def test_raises_file_missing (self):
        # Parameters
        path = "blablabla__wrong"
        verbose = False
        # Check if the right exception is raised.
        with self.assertRaises(RuntimeError) as re:
          ftl = blocks.file_taps_loader(path, verbose)
        self.assertEqual(str(re.exception), "file_taps_loader: Can not open " + "\"" + path + "\"" + ".")

if __name__ == '__main__':
    gr_unittest.run(qa_file_taps_loader, "qa_file_taps_loader.xml")
