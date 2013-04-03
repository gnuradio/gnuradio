#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2013 Free Software Foundation, Inc.
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

import random
import struct

"""
Note: There has been an issue with this block in the past, see Issue
#199. This test is being enabled only on the 'next' branch for version
v3.7 for now. TWR
"""

class counter(gr.feval_dd):
    def __init__(self, step_size=1):
        gr.feval_dd.__init__(self)
        self.step_size = step_size
        self.count = 0

    def eval(self, input):
        #print "eval: self.count =", self.count
        t = self.count
        self.count = self.count + self.step_size
        return t


class counter3(gr.feval_dd):
    def __init__(self, f, step_size):
        gr.feval_dd.__init__(self)
        self.f = f
        self.step_size = step_size
        self.count = 0

    def eval(self, input):
        try:
            #print "eval: self.count =", self.count
            t = self.count
            self.count = self.count + self.step_size
            self.f(self.count)
        except Exception, e:
            print "Exception: ", e
        return t

def foobar3(new_t):
    #print "foobar3: new_t =", new_t
    pass


class counter4(gr.feval_dd):
    def __init__(self, obj_instance, step_size):
        gr.feval_dd.__init__(self)
        self.obj_instance = obj_instance
        self.step_size = step_size
        self.count = 0

    def eval(self, input):
        try:
            #print "eval: self.count =", self.count
            t = self.count
            self.count = self.count + self.step_size
            self.obj_instance.foobar4(self.count)
        except Exception, e:
            print "Exception: ", e
        return t


class parse_msg(object):
    def __init__(self, msg):
        self.center_freq = msg.arg1()
        self.vlen = int(msg.arg2())
        assert(msg.length() == self.vlen * gr.sizeof_float)
        self.data = struct.unpack('%df' % (self.vlen,), msg.to_string())

class test_bin_statistics(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        vlen = 4
        tune = counter(1)
        tune_delay = 0
        dwell_delay = 1
        msgq = gr.msg_queue()

        src_data = tuple([float(x) for x in
                          ( 1,  2,  3,  4,
                            5,  6,  7,  8,
                            9, 10, 11, 12,
                            13, 14, 15, 16
                            )])

        expected_results = tuple([float(x) for x in
                                  ( 1,  2,  3,  4,
                                    5,  6,  7,  8,
                                    9, 10, 11, 12,
                                    13, 14, 15, 16
                                    )])

        src = blocks.vector_source_f(src_data, False)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vlen)
        stats = blocks.bin_statistics_f(vlen, msgq, tune, tune_delay, dwell_delay)
        self.tb.connect(src, s2v, stats)
        self.tb.run()
        self.assertEqual(4, msgq.count())
        for i in range(4):
            m = parse_msg(msgq.delete_head())
            #print "m =", m.center_freq, m.data
            self.assertEqual(expected_results[vlen*i:vlen*i + vlen], m.data)

    def test_002(self):
        vlen = 4
        tune = counter(1)
        tune_delay = 1
        dwell_delay = 2
        msgq = gr.msg_queue()

        src_data = tuple([float(x) for x in
                          ( 1,  2,  3,  4,
                            9,  6, 11,  8,
                            5, 10,  7, 12,
                            13, 14, 15, 16
                            )])

        expected_results = tuple([float(x) for x in
                                  ( 9, 10, 11, 12)])

        src = blocks.vector_source_f(src_data, False)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vlen)
        stats = blocks.bin_statistics_f(vlen, msgq, tune, tune_delay, dwell_delay)
        self.tb.connect(src, s2v, stats)
        self.tb.run()
        self.assertEqual(1, msgq.count())
        for i in range(1):
            m = parse_msg(msgq.delete_head())
            #print "m =", m.center_freq, m.data
            self.assertEqual(expected_results[vlen*i:vlen*i + vlen], m.data)



    def test_003(self):
        vlen = 4
        tune = counter3(foobar3, 1)
        tune_delay = 1
        dwell_delay = 2
        msgq = gr.msg_queue()

        src_data = tuple([float(x) for x in
                          ( 1,  2,  3,  4,
                            9,  6, 11,  8,
                            5, 10,  7, 12,
                            13, 14, 15, 16
                            )])

        expected_results = tuple([float(x) for x in
                                  ( 9, 10, 11, 12)])

        src = blocks.vector_source_f(src_data, False)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vlen)
        stats = blocks.bin_statistics_f(vlen, msgq, tune, tune_delay, dwell_delay)
        self.tb.connect(src, s2v, stats)
        self.tb.run()
        self.assertEqual(1, msgq.count())
        for i in range(1):
            m = parse_msg(msgq.delete_head())
            #print "m =", m.center_freq, m.data
            self.assertEqual(expected_results[vlen*i:vlen*i + vlen], m.data)

    def foobar4(self, new_t):
        #print "foobar4: new_t =", new_t
        pass

    def test_004(self):
        vlen = 4
        tune = counter4(self, 1)
        tune_delay = 1
        dwell_delay = 2
        msgq = gr.msg_queue()

        src_data = tuple([float(x) for x in
                          ( 1,  2,  3,  4,
                            9,  6, 11,  8,
                            5, 10,  7, 12,
                            13, 14, 15, 16
                            )])

        expected_results = tuple([float(x) for x in
                                  ( 9, 10, 11, 12)])

        src = blocks.vector_source_f(src_data, False)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vlen)
        stats = blocks.bin_statistics_f(vlen, msgq, tune, tune_delay, dwell_delay)
        self.tb.connect(src, s2v, stats)
        self.tb.run()
        self.assertEqual(1, msgq.count())
        for i in range(1):
            m = parse_msg(msgq.delete_head())
            #print "m =", m.center_freq, m.data
            self.assertEqual(expected_results[vlen*i:vlen*i + vlen], m.data)


if __name__ == '__main__':
   gr_unittest.run(test_bin_statistics, "test_bin_statistics.xml")
