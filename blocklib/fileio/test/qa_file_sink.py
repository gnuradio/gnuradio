#!/usr/bin/env python3
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
from gnuradio import gr, gr_unittest, blocks, fileio


class test_file_sink(gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.flowgraph()
        self.rt = gr.runtime()

    def tearDown(self):
        self.tb = None
        self.rt = None

    def test_file_sink(self):
        data = range(1000)
        expected_result = data

        with tempfile.NamedTemporaryFile() as temp:
            src = blocks.vector_source_f(data)
            snk = fileio.file_sink(temp.name)
            snk.set_unbuffered(True) # FIXME: comes from base class no pybind yet
            self.tb.connect(src, snk)
            self.rt.initialize(self.tb)
            self.rt.run()

            # Check file length (float: 4 * nsamples)
            file_size = os.stat(temp.name).st_size
            self.assertEqual(file_size, 4 * len(data))

            # Check file contents
            with open(temp.name, 'rb') as datafile:
                result_data = array.array('f')
                result_data.fromfile(datafile, len(data))
                self.assertFloatTuplesAlmostEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(test_file_sink)
