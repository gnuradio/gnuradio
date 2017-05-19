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

from gnuradio import gr, gr_unittest, blocks
import os
import tempfile
import pmt

class test_file_source_sink(gr_unittest.TestCase):

    def setUp (self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        src_data = range(1000)
        expected_result = range(1000)

        snk2 = blocks.vector_sink_f()

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(src_data)
            snk = blocks.file_sink(gr.sizeof_float, temp.name)
            snk.set_unbuffered(True)

            src2 = blocks.file_source(gr.sizeof_float, temp.name)

            self.tb.connect(src, snk)
            self.tb.run()

            self.tb.disconnect(src, snk)
            self.tb.connect(src2, snk2)
            self.tb.run()

        result_data = snk2.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk2.tags()), 0)

    def test_descriptor_001(self):
        src_data = range(1000)
        expected_result = range(1000)

        snk2 = blocks.vector_sink_f()

        with tempfile.NamedTemporaryFile() as temp:
            fhandle0 = open(temp.name, "wb")
            fd0 = fhandle0.fileno()

            src = blocks.vector_source_f(src_data)
            snk = blocks.file_descriptor_sink(gr.sizeof_float, fd0)

            self.tb.connect(src, snk)
            self.tb.run()
            os.fsync(fd0)
            fhandle0.close()

            fhandle1 = open(temp.name, "rb")
            fd1 = fhandle1.fileno()
            src2 = blocks.file_descriptor_source(gr.sizeof_float, fd1, False)

            self.tb.disconnect(src, snk)
            self.tb.connect(src2, snk2)
            self.tb.run()
            os.fsync(fd1)
            fhandle1.close()

        result_data = snk2.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk2.tags()), 0)

    def test_file_source_can_seek_after_open(self):
        src_data = range(1000)

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(src_data)
            snk = blocks.file_sink(gr.sizeof_float, temp.name)
            snk.set_unbuffered(True)

            self.tb.connect(src, snk)
            self.tb.run()

            source = blocks.file_source(gr.sizeof_float, temp.name)
            self.assertTrue(source.seek(0, os.SEEK_SET))

    def test_begin_tag(self):
        src_data = range(1000)
        expected_result = range(1000)

        snk2 = blocks.vector_sink_f()

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(src_data)
            snk = blocks.file_sink(gr.sizeof_float, temp.name)
            snk.set_unbuffered(True)

            src2 = blocks.file_source(gr.sizeof_float, temp.name)
            src2.set_begin_tag(pmt.string_to_symbol("file_begin"))

            self.tb.connect(src, snk)
            self.tb.run()

            self.tb.disconnect(src, snk)
            self.tb.connect(src2, snk2)
            self.tb.run()

        result_data = snk2.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk2.tags()), 1)

    def test_begin_tag_repeat(self):
        src_data = range(1000)
        expected_result = range(1000)
        expected_result.extend(range(1000))

        snk2 = blocks.vector_sink_f()

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(src_data)
            snk = blocks.file_sink(gr.sizeof_float, temp.name)
            snk.set_unbuffered(True)

            src2 = blocks.file_source(gr.sizeof_float, temp.name, True)
            src2.set_begin_tag(pmt.string_to_symbol("file_begin"))
            hd = blocks.head(gr.sizeof_float, 2000)

            self.tb.connect(src, snk)
            self.tb.run()

            self.tb.disconnect(src, snk)
            self.tb.connect(src2, hd, snk2)
            self.tb.run()

        result_data = snk2.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        tags = snk2.tags()
        self.assertEqual(len(tags), 2)
        self.assertEqual(str(tags[0].key), "file_begin")
        self.assertEqual(str(tags[0].value), "0")
        self.assertEqual(tags[0].offset, 0)
        self.assertEqual(str(tags[1].key), "file_begin")
        self.assertEqual(str(tags[1].value), "1")
        self.assertEqual(tags[1].offset, 1000)

if __name__ == '__main__':
    gr_unittest.run(test_file_source_sink, "test_file_source_sink.xml")

