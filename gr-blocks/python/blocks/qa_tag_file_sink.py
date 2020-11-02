#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import os
import struct


class test_tag_file_sink(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        trg_data = [-1, -1, 1, 1, -1, -1, 1, 1, -1, -1]
        src = blocks.vector_source_i(src_data)
        trg = blocks.vector_source_s(trg_data)
        op = blocks.burst_tagger(gr.sizeof_int)
        snk = blocks.tagged_file_sink(gr.sizeof_int, 1)
        self.tb.connect(src, (op, 0))
        self.tb.connect(trg, (op, 1))
        self.tb.connect(op, snk)
        self.tb.run()

        # Tagged file sink gets 2 burst tags at index 2 and index 5.
        # Creates two new files, each with two integers in them from
        # src_data at these indexes (3,4) and (7,8).
        file0 = "file{0}_0_2.00000000.dat".format(snk.unique_id())
        file1 = "file{0}_1_6.00000000.dat".format(snk.unique_id())

        # Open the files and read in the data, then remove the files
        # to clean up the directory.
        outfile0 = open(file0, 'rb')
        outfile1 = open(file1, 'rb')
        data0 = outfile0.read(8)
        data1 = outfile1.read(8)
        outfile0.close()
        outfile1.close()
        os.remove(file0)
        os.remove(file1)

        # Convert the 8 bytes from the files into a tuple of 2 ints.
        idata0 = struct.unpack('ii', data0)
        idata1 = struct.unpack('ii', data1)

        self.assertEqual(idata0, (3, 4))
        self.assertEqual(idata1, (7, 8))


if __name__ == '__main__':
    gr_unittest.run(test_tag_file_sink)
