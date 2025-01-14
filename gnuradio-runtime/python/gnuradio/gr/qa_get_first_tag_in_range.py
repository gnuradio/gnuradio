#!/usr/bin/env python
#
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
        gr.sync_block.__init__(self, "test block", out_sig=[], in_sig=[numpy.float32])
        self.assert_function = assert_function
        self.set_output_multiple(MULTIPLE)
        self.counter = 0

    def work(self, input_items, _):
        read = self.nitems_read(0)
        tag = self.get_first_tag_in_range(0, read, read + MULTIPLE, KEY)
        self.assert_function(tag is not None, "got no tag")

        self.assert_function(
            pmt.to_long(tag.value) == MULTIPLE * self.counter,
            f"value at {tag.offset} wrong ({tag.value})",
        )
        self.assert_function(tag.offset % MULTIPLE == 12, f"offset {tag.offset} wrong")
        tag = self.get_first_tag_in_range(0, tag.offset + 1, read + MULTIPLE, KEY)
        self.assert_function(
            not tag,
            f"should only have one tag, got second at {tag.offset if tag else '???'}",
        )
        self.counter += 1
        return MULTIPLE


class first_tag_predicate(gr.sync_block):
    def __init__(self, assert_function):
        gr.sync_block.__init__(self, "test block", out_sig=[], in_sig=[numpy.float32])
        self.assert_function = assert_function
        self.set_output_multiple(MULTIPLE)

    def work(self, input_items, _):
        read = self.nitems_read(0)
        tag = self.get_first_tag_in_range(
            0,
            read,
            read + MULTIPLE,
            lambda tag: tag.key == KEY and tag.offset % MULTIPLE == 12,
        )
        definitely_no_tag = self.get_first_tag_in_range(
            0, read, read + MULTIPLE, lambda tag: False
        )
        self.assert_function(tag is not None, "got no tag, though there should be one")
        self.assert_function(
            definitely_no_tag is None, "got a tag where I definitely shouldn't"
        )
        return MULTIPLE


class source_block(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(self, "test block", in_sig=[], out_sig=[numpy.float32])

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


class test_first_tag_in_range(gr_unittest.TestCase):

    def test_001_filter_by_key(self):
        tb = gr.top_block(catch_exceptions=False)
        src = source_block()
        sink = first_tag_key(lambda truth, msg: self.assertTrue(truth, msg))
        tb.connect(src, sink)
        tb.run(max_noutput_items=1000000)

    def test_002_filter_by_predicate(self):
        tb = gr.top_block(catch_exceptions=False)
        src = source_block()
        sink = first_tag_predicate(lambda truth, msg: self.assertTrue(truth, msg))
        tb.connect(src, sink)
        tb.run(max_noutput_items=1000000)


if __name__ == "__main__":
    gr_unittest.run(test_first_tag_in_range)
