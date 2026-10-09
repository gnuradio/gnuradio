#!/usr/bin/env python
#
# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import blocks, gr, gr_unittest, qtgui

try:
    import sip
except ImportError:
    from PyQt5 import sip

from PyQt5 import QtWidgets

item_type_t = qtgui.number_sink.item_type_t

# The sink aligns its input on a VOLK boundary, so for single byte items the
# scheduler only runs it once a fair number of items have piled up.
NITEMS = 256


class test_number_sink(gr_unittest.TestCase):

    def make_sink(self, itemtype_or_size, nconnections=1):
        return qtgui.number_sink(itemtype_or_size, 0,
                                 qtgui.NUM_GRAPH_NONE, nconnections, None)

    def value_box(self, sink):
        """The label showing the value of the first stream.

        NumberDisplayForm arranges its widgets in a grid. Without a bar
        graph and without a title, the value of stream i sits at row 1,
        column i, below its name label.
        """
        widget = sip.wrapinstance(sink.qwidget(), QtWidgets.QWidget)
        item = widget.layout().itemAtPosition(1, 0)
        self.assertIsNotNone(item, "no widget at the value position")
        box = item.widget()
        self.assertIsInstance(box, QtWidgets.QLabel)
        return box

    def displayed_text(self, itemtype_or_size, source):
        """Run a one source flowgraph and return the text the sink shows."""
        sink = self.make_sink(itemtype_or_size)
        tb = gr.top_block()
        tb.connect(source, sink)
        tb.run()
        # run() returns once the sink has posted its update, so a single
        # pass over the event queue is enough to deliver it.
        QtWidgets.QApplication.instance().processEvents()
        return self.value_box(sink).text()

    def display(self, itemtype_or_size, source):
        return float(self.displayed_text(itemtype_or_size, source))

    def test_item_type_sizes(self):
        for itemtype, itemsize in ((item_type_t.FLOAT32, gr.sizeof_float),
                                   (item_type_t.INT32, gr.sizeof_int),
                                   (item_type_t.INT16, gr.sizeof_short),
                                   (item_type_t.INT8, gr.sizeof_char)):
            with self.subTest(itemtype=itemtype):
                sink = self.make_sink(itemtype)
                self.assertEqual(
                    itemsize, sink.input_signature().sizeof_stream_item(0))

    def test_nconnections(self):
        sink = self.make_sink(item_type_t.INT32, nconnections=3)
        self.assertEqual(3, sink.input_signature().max_streams())

    def test_float32_values(self):
        for value in (0.0, 1.5, -2.25, 42.0):
            with self.subTest(value=value):
                source = blocks.vector_source_f([value] * NITEMS)
                self.assertAlmostEqual(
                    value, self.display(item_type_t.FLOAT32, source), places=3)

    def test_int32_values(self):
        for value in (0, 1, -1, 42, 123456, -123456):
            with self.subTest(value=value):
                source = blocks.vector_source_i([value] * NITEMS)
                self.assertEqual(
                    value, self.display(item_type_t.INT32, source))

    def test_int16_values(self):
        for value in (0, 1, -1, 32767, -32768):
            with self.subTest(value=value):
                source = blocks.vector_source_s([value] * NITEMS)
                self.assertEqual(
                    value, self.display(item_type_t.INT16, source))

    def test_int8_values(self):
        # vector_source_b carries unsigned bytes, so the negative cases are
        # written as the two's complement patterns the sink must read back as
        # signed values, the way every other byte block in the tree does.
        for raw, value in ((0, 0), (1, 1), (42, 42), (127, 127),
                           (255, -1), (128, -128)):
            with self.subTest(raw=raw):
                source = blocks.vector_source_b([raw] * NITEMS)
                self.assertEqual(
                    value, self.display(item_type_t.INT8, source))

    def test_int32_is_not_read_as_float32(self):
        # int32 and float32 items are the same size, so the type has to come
        # from the constructor. Read as float32 these bit patterns are
        # denormals displayed as zero, not the integers they stand for.
        for value in (42, 16777216):
            with self.subTest(value=value):
                source = blocks.vector_source_i([value] * NITEMS)
                self.assertEqual(
                    float(value), self.display(item_type_t.INT32, source))

    def test_display_format_follows_item_type(self):
        # The same number is rendered as an integer or as a float depending
        # only on the item type, never on the widget's own state.
        as_int = self.displayed_text(
            item_type_t.INT32, blocks.vector_source_i([42] * NITEMS))
        as_float = self.displayed_text(
            item_type_t.FLOAT32, blocks.vector_source_f([42.0] * NITEMS))
        self.assertEqual(42.0, float(as_int))
        self.assertEqual(42.0, float(as_float))
        self.assertNotEqual(as_int, as_float)

    def test_itemsize_sizes(self):
        for itemsize in (gr.sizeof_char, gr.sizeof_short, gr.sizeof_float):
            with self.subTest(itemsize=itemsize):
                sink = self.make_sink(itemsize)
                self.assertEqual(
                    itemsize, sink.input_signature().sizeof_stream_item(0))

    def test_itemsize_four_bytes_stays_float32(self):
        source = blocks.vector_source_f([1.5] * NITEMS)
        self.assertAlmostEqual(
            1.5, self.display(gr.sizeof_float, source), places=3)

    def test_itemsize_rejects_unsupported_size(self):
        for itemsize in (0, 3, gr.sizeof_double):
            with self.subTest(itemsize=itemsize):
                self.assertRaises(RuntimeError, self.make_sink, itemsize)


if __name__ == '__main__':
    gr_unittest.run(test_number_sink)
