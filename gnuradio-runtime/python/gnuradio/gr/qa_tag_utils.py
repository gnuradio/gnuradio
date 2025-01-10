#!/usr/bin/env python
#
# Copyright 2007,2010 Free Software Foundation, Inc.
# Copyright 2021,2025 Marcus Müller <mmueller@gnuradio.org>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
import pmt
import numpy

MULTIPLE = 100
KEY = pmt.string_to_symbol("key")


class first_tag_key(gr.sync_block):
    def __init__(self, assert_function):
        gr.sync_block.__init__(
            self, "test block", out_sig=[], in_sig=[numpy.float32])
        self.assert_function = assert_function
        self.set_output_multiple(MULTIPLE)
        self.counter = 0

    def work(self, input_items, _):
        read = self.nitems_read(0)
        tag = self.get_first_tag_in_range(0,
                                          read,
                                          read + MULTIPLE,
                                          KEY)
        self.assert_function(
            tag is not None, "got no tag")

        self.assert_function(pmt.to_long(tag.value) == MULTIPLE * self.counter,
                             f"value at {tag.offset} wrong ({tag.value})")
        self.assert_function(tag.offset % MULTIPLE == 12,
                             f"offset {tag.offset} wrong")
        tag = self.get_first_tag_in_range(0,
                                          tag.offset + 1,
                                          read + MULTIPLE,
                                          KEY)
        self.assert_function(not tag, f"should only have one tag, got second at {tag.offset if tag else '???'}")
        self.counter += 1
        return MULTIPLE


class first_tag_predicate(gr.sync_block):
    def __init__(self, assert_function):
        gr.sync_block.__init__(
            self, "test block", out_sig=[], in_sig=[numpy.float32])
        self.assert_function = assert_function
        self.set_output_multiple(MULTIPLE)

    def work(self, input_items, _):
        read = self.nitems_read(0)
        tag = self.get_first_tag_in_range(0,
                                          read,
                                          read + MULTIPLE,
                                          lambda tag: tag.key == KEY and tag.offset % MULTIPLE == 12)
        definitely_no_tag = self.get_first_tag_in_range(0,
                                                        read,
                                                        read + MULTIPLE,
                                                        lambda tag: False)
        self.assert_function(tag is not None, "got no tag, though there should be one")
        self.assert_function(definitely_no_tag is None, "got a tag where I definitely shouldn't")
        return MULTIPLE


class source_block(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self, "test block", in_sig=[], out_sig=[numpy.float32])

    def work(self, _, output_items):
        if self.nitems_written(0) > 100000:
            return -1
        nout = len(output_items[0])
        for idx in range(self.nitems_written(0), self.nitems_written(0) + nout):
            counter = idx // MULTIPLE
            if idx % MULTIPLE == 12:
                tag = gr.tag_t()
                tag.offset = idx
                tag.key = KEY
                tag.value = pmt.from_long(counter * MULTIPLE)
                self.add_item_tag(0, tag)
        return nout


class test_tag_utils (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
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

    def test_comparison(self):
        t = gr.tag_t()
        t.offset = 10
        t.key = pmt.string_to_symbol('key')
        t.value = pmt.from_long(23)
        t.srcid = pmt.from_bool(False)

        t2 = gr.tag_t()
        t2.offset = 100
        t2.key = pmt.string_to_symbol('aaa')
        t2.value = pmt.from_long(230)
        t2.srcid = pmt.from_bool(True)
        self.assertTrue(t < t2)
        self.assertTrue(t == t)
        self.assertTrue(t != t2)
        self.assertFalse(t > t2)
        self.assertFalse(t < t)

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

        for k, t in zip(sorted(offsets), sorted(tags)):
            self.assertEqual(t.offset, k)
            self.assertTrue(pmt.equal(t.key, key))
            self.assertTrue(pmt.equal(t.value, pmt.from_long(k)))
            self.assertTrue(pmt.equal(t.srcid, srcid))

        tmin = min(tags)
        self.assertEqual(tmin.offset, min(offsets))
        self.assertTrue(pmt.equal(tmin.key, key))
        self.assertTrue(pmt.equal(tmin.value, pmt.from_long(min(offsets))))
        self.assertTrue(pmt.equal(tmin.srcid, srcid))

        tmax = max(tags)
        self.assertEqual(tmax.offset, max(offsets))
        self.assertTrue(pmt.equal(tmax.key, key))
        self.assertTrue(pmt.equal(tmax.value, pmt.from_long(max(offsets))))
        self.assertTrue(pmt.equal(tmax.srcid, srcid))

    def test_004_get_first_tag_in_range(self):
        tb = gr.top_block(catch_exceptions=False)
        src = source_block()
        sink = first_tag_key(lambda truth, msg: self.assertTrue(truth, msg))
        sink2 = first_tag_predicate(lambda truth, msg: self.assertTrue(truth, msg))
        tb.connect(src, sink)
        tb.connect(src, sink2)
        tb.run()


if __name__ == '__main__':
    gr_unittest.run(test_tag_utils)
