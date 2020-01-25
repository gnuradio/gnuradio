#!/usr/bin/env python
#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import os
import tempfile
import array
from gnuradio import gr, gr_unittest, blocks

class test_file_sink(gr_unittest.TestCase):

    def setUp (self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_file_sink(self):
        data = range(1000)
        expected_result = data

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(data)
            snk = blocks.file_sink(gr.sizeof_float, temp.name)
            snk.set_unbuffered(True)
            self.tb.connect(src, snk)
            self.tb.run()

            # Check file length (float: 4 * nsamples)
            file_size = os.stat(temp.name).st_size
            self.assertEqual(file_size, 4 * len(data))

            # Check file contents
            datafile = open(temp.name, 'rb')
            result_data = array.array('f')
            result_data.fromfile(datafile, len(data))
            self.assertFloatTuplesAlmostEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_file_sink, "test_file_sink.xml")
