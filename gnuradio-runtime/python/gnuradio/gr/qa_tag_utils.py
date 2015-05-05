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

try:
    import pmt_swig as pmt
except ImportError:
    import pmt

class test_tag_utils (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()


    def tearDown (self):
        self.tb = None

    def test_001(self):
        t = gr.tag_t()
        t.offset = 10
        t.key = pmt.string_to_symbol('key')
        t.value = pmt.from_long(23)
        t.srcid = pmt.from_bool(False)
        pt = gr.tag_to_python(t)
        self.assertEqual(pt.key, 'key')
        self.assertEqual(pt.value, 23)
        self.assertEqual(pt.offset, 10)

    def test_002(self):
        offset = 10
        key = pmt.string_to_symbol('key')
        value = pmt.from_long(23)
        srcid = pmt.from_bool(False)

        format_dict = {'offset': offset,
                       'key': key,
                       'value': value,
                       'srcid': srcid}
        format_list = [offset, key, value, srcid]
        format_tuple = (offset, key, value, srcid)

        t_dict = gr.python_to_tag(format_dict)
        t_list = gr.python_to_tag(format_list)
        t_tuple = gr.python_to_tag(format_tuple)

        self.assertTrue(pmt.equal(t_dict.key, key))
        self.assertTrue(pmt.equal(t_dict.value, value))
        self.assertEqual(t_dict.offset, offset)

        self.assertTrue(pmt.equal(t_list.key, key))
        self.assertTrue(pmt.equal(t_list.value, value))
        self.assertEqual(t_list.offset, offset)

        self.assertTrue(pmt.equal(t_tuple.key, key))
        self.assertTrue(pmt.equal(t_tuple.value, value))
        self.assertEqual(t_tuple.offset, offset)

    def test_003(self):
        offsets = (6, 3, 8)
        key = pmt.string_to_symbol('key')
        srcid = pmt.string_to_symbol('qa_tag_utils')
        tags = []

        for k in offsets:
            t = gr.tag_t()
            t.offset = k
            t.key = key
            t.value = pmt.from_long(k)
            t.srcid = srcid
            tags.append(t)

        for k, t in zip(sorted(offsets),
                        sorted(tags, key=gr.tag_t_offset_compare_key())):
            self.assertEqual(t.offset, k)
            self.assertTrue(pmt.equal(t.key, key))
            self.assertTrue(pmt.equal(t.value, pmt.from_long(k)))
            self.assertTrue(pmt.equal(t.srcid, srcid))

        tmin = min(tags, key=gr.tag_t_offset_compare_key())
        self.assertEqual(tmin.offset, min(offsets))
        self.assertTrue(pmt.equal(tmin.key, key))
        self.assertTrue(pmt.equal(tmin.value, pmt.from_long(min(offsets))))
        self.assertTrue(pmt.equal(tmin.srcid, srcid))

        tmax = max(tags, key=gr.tag_t_offset_compare_key())
        self.assertEqual(tmax.offset, max(offsets))
        self.assertTrue(pmt.equal(tmax.key, key))
        self.assertTrue(pmt.equal(tmax.value, pmt.from_long(max(offsets))))
        self.assertTrue(pmt.equal(tmax.srcid, srcid))


if __name__ == '__main__':
    print 'hi'
    gr_unittest.run(test_tag_utils, "test_tag_utils.xml")

