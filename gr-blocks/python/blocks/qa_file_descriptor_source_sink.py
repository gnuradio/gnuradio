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

    def setUp (self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_file_descriptor(self):
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

if __name__ == '__main__':
    gr_unittest.run(test_file_descriptor_source_sink, "test_file_descriptor_source_sink.xml")
