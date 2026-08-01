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

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block()
        temp = tempfile.NamedTemporaryFile(delete=False)
        temp.close()
        self._datafilename = temp.name

    def tearDown(self):
        self.tb = None
        os.unlink(self._datafilename)

    def test_file_sink(self):
        data = range(1000)
        expected_result = data

        src = blocks.vector_source_f(data)
        snk = blocks.file_sink(gr.sizeof_float, self._datafilename)
        snk.set_unbuffered(True)
        self.tb.connect(src, snk)
        self.tb.run()
        snk.close()

        # Check file length (float: 4 * nsamples)
        file_size = os.stat(self._datafilename).st_size
        self.assertEqual(file_size, 4 * len(data))

        # Check file contents
        result_data = array.array('f')
        with open(self._datafilename, 'rb') as datafile:
            result_data.fromfile(datafile, len(data))
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_fail_if_exists_raises_when_file_exists(self):
        # self._datafilename was already created by setUp via
        # NamedTemporaryFile, so this must fail immediately.
        self.assertRaises(
            RuntimeError,
            blocks.file_sink,
            gr.sizeof_float,
            self._datafilename,
            False,
            True,
        )

    def test_fail_if_exists_succeeds_for_new_file(self):
        os.unlink(self._datafilename)

        data = range(1000)
        src = blocks.vector_source_f(data)
        snk = blocks.file_sink(gr.sizeof_float, self._datafilename, False, True)
        snk.set_unbuffered(True)
        self.tb.connect(src, snk)
        self.tb.run()
        snk.close()

        file_size = os.stat(self._datafilename).st_size
        self.assertEqual(file_size, 4 * len(data))

    def test_fail_if_exists_false_still_overwrites(self):
        # default (fail_if_exists=False) must keep working exactly as before
        # on a file that already exists.
        data = range(10)
        src = blocks.vector_source_f(data)
        snk = blocks.file_sink(gr.sizeof_float, self._datafilename, False, False)
        snk.set_unbuffered(True)
        self.tb.connect(src, snk)
        self.tb.run()
        snk.close()

        file_size = os.stat(self._datafilename).st_size
        self.assertEqual(file_size, 4 * len(data))


if __name__ == '__main__':
    gr_unittest.run(test_file_sink)
