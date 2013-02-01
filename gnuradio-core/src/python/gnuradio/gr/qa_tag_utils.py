#!/usr/bin/env python
#
# Copyright 2007,2010 Free Software Foundation, Inc.
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
import tag_utils

try: from gruel import pmt
except: import pmt

class test_tag_utils (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()


    def tearDown (self):
        self.tb = None

    def test_001(self):
        t = gr.gr_tag_t()
        t.offset = 10
        t.key = pmt.pmt_string_to_symbol('key')
        t.value = pmt.pmt_from_long(23)
        t.srcid = pmt.pmt_from_bool(False)
        pt = tag_utils.tag_to_python(t)
        self.assertEqual(pt.key, 'key')
        self.assertEqual(pt.value, 23)
        self.assertEqual(pt.offset, 10)


if __name__ == '__main__':
    print 'hi'
    gr_unittest.run(test_tag_utils, "test_tag_utils.xml")

