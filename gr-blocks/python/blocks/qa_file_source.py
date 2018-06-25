#!/usr/bin/env python
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

import os
import tempfile
import array
import pmt
from gnuradio import gr, gr_unittest, blocks

class test_file_source(gr_unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        cls._datafile = tempfile.NamedTemporaryFile()
        cls._datafilename = cls._datafile.name
        cls._vector = [x for x in range(1000)]
        with open(cls._datafilename, 'wb') as f:
            array.array('f', cls._vector).tofile(f)

    @classmethod
    def tearDownClass(cls):
        del cls._vector
        del cls._datafilename
        del cls._datafile

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test_file_source(self):
        src = blocks.file_source(gr.sizeof_float, self._datafilename)
        snk = blocks.vector_sink_f()
        self.tb.connect(src, snk)
        self.tb.run()

        result_data = snk.data()
        self.assertFloatTuplesAlmostEqual(self._vector, result_data)
        self.assertEqual(len(snk.tags()), 0)

    def test_file_source_no_such_file(self):
        """
        Try to open a non-existant file and verify exception is thrown.
        """
        try:
            _ = blocks.file_source(gr.sizeof_float, "___no_such_file___")
            self.assertTrue(False)
        except RuntimeError:
            self.assertTrue(True)

    def test_file_source_with_offset(self):
        expected_result = self._vector[100:]

        src = blocks.file_source(gr.sizeof_float, self._datafilename, offset=100)
        snk = blocks.vector_sink_f()

        self.tb.connect(src, snk)
        self.tb.run()

        result_data = snk.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk.tags()), 0)

    def test_source_with_offset_and_len(self):
        expected_result = self._vector[100:100+600]

        src = blocks.file_source(gr.sizeof_float, self._datafilename, offset=100, len=600)
        snk = blocks.vector_sink_f()
        self.tb.connect(src, snk)
        self.tb.run()

        result_data = snk.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk.tags()), 0)

    def test_file_source_can_seek_after_open(self):

        src = blocks.file_source(gr.sizeof_float, self._datafilename)
        self.assertTrue(src.seek(0, os.SEEK_SET))
        self.assertTrue(src.seek(len(self._vector)-1, os.SEEK_SET))
        # Seek past end of file - this will also log a warning
        self.assertFalse(src.seek(len(self._vector), os.SEEK_SET))
        # Negative seek - this will also log a warning
        self.assertFalse(src.seek(-1, os.SEEK_SET))

        self.assertTrue(src.seek(1, os.SEEK_END))
        self.assertTrue(src.seek(len(self._vector), os.SEEK_END))
        # Seek past end of file - this will also log a warning
        self.assertFalse(src.seek(0, os.SEEK_END))

        self.assertTrue(src.seek(0, os.SEEK_SET))
        self.assertTrue(src.seek(1, os.SEEK_CUR))
        # Seek past end of file - this will also log a warning
        self.assertFalse(src.seek(len(self._vector), os.SEEK_CUR))


    def test_begin_tag(self):
        expected_result = self._vector

        src = blocks.file_source(gr.sizeof_float, self._datafilename)
        src.set_begin_tag(pmt.string_to_symbol("file_begin"))
        snk = blocks.vector_sink_f()
        self.tb.connect(src, snk)
        self.tb.run()

        result_data = snk.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk.tags()), 1)

    def test_begin_tag_repeat(self):
        expected_result = self._vector + self._vector

        src = blocks.file_source(gr.sizeof_float, self._datafilename, True)
        src.set_begin_tag(pmt.string_to_symbol("file_begin"))
        head = blocks.head(gr.sizeof_float, 2 * len(self._vector))
        snk = blocks.vector_sink_f()
        self.tb.connect(src, head, snk)
        self.tb.run()

        result_data = snk.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        tags = snk.tags()
        self.assertEqual(len(tags), 2)
        self.assertEqual(str(tags[0].key), "file_begin")
        self.assertEqual(str(tags[0].value), "0")
        self.assertEqual(tags[0].offset, 0)
        self.assertEqual(str(tags[1].key), "file_begin")
        self.assertEqual(str(tags[1].value), "1")
        self.assertEqual(tags[1].offset, 1000)

if __name__ == '__main__':
    gr_unittest.run(test_file_source, "test_file_source.xml")
