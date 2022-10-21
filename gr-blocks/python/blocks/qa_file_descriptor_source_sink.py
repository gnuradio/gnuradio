#!/usr/bin/env python
#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, blocks
import os
import tempfile
import pmt


class test_file_descriptor_source_sink(gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block()
        temp = tempfile.NamedTemporaryFile(delete=False)
        temp.close()
        self._datafilename = temp.name

    def tearDown(self):
        self.tb = None
        os.unlink(self._datafilename)

    def test_file_descriptor(self):
        src_data = range(1000)
        expected_result = range(1000)

        snk2 = blocks.vector_sink_f()

        binary = os.O_BINARY if os.name == "nt" else 0
        fd0 = os.open(self._datafilename, os.O_WRONLY | binary)

        src = blocks.vector_source_f(src_data)
        snk = blocks.file_descriptor_sink(gr.sizeof_float, fd0)

        self.tb.connect(src, snk)
        self.tb.run()

        fd1 = os.open(self._datafilename, os.O_RDONLY | binary)
        src2 = blocks.file_descriptor_source(gr.sizeof_float, fd1, False)

        self.tb.disconnect(src, snk)
        self.tb.connect(src2, snk2)
        self.tb.run()

        result_data = snk2.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)
        self.assertEqual(len(snk2.tags()), 0)


if __name__ == '__main__':
    gr_unittest.run(test_file_descriptor_source_sink)
