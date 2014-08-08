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

import sys, time, random, numpy
from gnuradio import gr, gr_unittest, blocks

import os, struct

class test_ctrlport_probes(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'True'

    def tearDown(self):
        self.tb = None

    def xtest_001(self):
        pass

    def test_002(self):
        pass


    def test_003(self):
        pass

    def test_004(self):
        pass

    def test_005(self):
        pass

if __name__ == '__main__':
    gr_unittest.run(test_ctrlport_probes, "test_ctrlport_probes.xml")
