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
