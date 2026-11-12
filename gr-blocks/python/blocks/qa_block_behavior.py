#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy

from gnuradio import gr, gr_unittest, blocks


class largest_call_probe(gr.sync_block):
    '''
    Float pass-through that records the largest noutput_items the scheduler
    ever handed to work().
    '''

    def __init__(self):
        gr.sync_block.__init__(
            self, "largest_call_probe", [numpy.float32], [numpy.float32])
        self.largest = 0

    def work(self, input_items, output_items):
        n = len(output_items[0])
        self.largest = max(self.largest, n)
        output_items[0][:] = input_items[0][:n]
        return n


class test_block_behavior(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        '''
        Tests the max noutput sizes set by the scheduler. When creating
        the block, there is no block_detail and so the max buffer size
        is 0. When the top_block is run, it builds the detail and
        buffers and sets the max value. test_0001 tests when the
        max_noutput_items is set by hand.

        '''

        src = blocks.null_source(gr.sizeof_float)
        op = blocks.head(gr.sizeof_float, 100)
        snk = blocks.null_sink(gr.sizeof_float)

        maxn_pre = op.max_noutput_items()

        self.tb.connect(src, op, snk)
        self.tb.run()

        maxn_post = op.max_noutput_items()

        self.assertEqual(maxn_pre, 0)
        self.assertEqual(maxn_post, 16384)
        # The value is derived from the allocated buffer, not an explicit
        # per-block limit, so the scheduler still applies the top_block
        # maximum.
        self.assertFalse(op.is_set_max_noutput_items())

    def test_001(self):
        '''
        Tests the max noutput size when being explicitly set.
        '''

        src = blocks.null_source(gr.sizeof_float)
        op = blocks.head(gr.sizeof_float, 100)
        snk = blocks.null_sink(gr.sizeof_float)

        op.set_max_noutput_items(1024)

        maxn_pre = op.max_noutput_items()

        self.tb.connect(src, op, snk)
        self.tb.run()

        maxn_post = op.max_noutput_items()

        self.assertEqual(maxn_pre, 1024)
        self.assertEqual(maxn_post, 1024)

    def run_probe(self, probe, run_max_noutput_items=None):
        '''
        Pushes enough items through the probe that the scheduler would
        hand it full-buffer calls unless something caps them.
        '''
        src = blocks.null_source(gr.sizeof_float)
        op = blocks.head(gr.sizeof_float, 200000)
        snk = blocks.null_sink(gr.sizeof_float)
        self.tb.connect(src, op, probe, snk)
        if run_max_noutput_items is None:
            self.tb.run()
        else:
            self.tb.run(run_max_noutput_items)

    def test_002(self):
        '''
        Tests that the max_noutput_items argument of top_block.run()
        caps work() calls of a block that has outputs and did not set
        its own maximum.
        '''

        probe = largest_call_probe()
        self.run_probe(probe, 64)

        # Guards the upper bound below against work() never running.
        self.assertGreater(probe.largest, 0)
        self.assertLessEqual(probe.largest, 64)
        self.assertFalse(probe.is_set_max_noutput_items())

    def test_003(self):
        '''
        Tests that a block's own max_noutput_items takes precedence over
        the max_noutput_items argument of top_block.run().
        '''

        probe = largest_call_probe()
        probe.set_max_noutput_items(64)
        self.run_probe(probe, 32)

        # The block limit is the larger one, so calls above the top_block
        # limit show the block limit replaced it rather than combining
        # with it as min(64, 32).
        self.assertGreater(probe.largest, 32)
        self.assertLessEqual(probe.largest, 64)
        self.assertEqual(probe.max_noutput_items(), 64)

    def test_004(self):
        '''
        Tests that a max output buffer caps work() calls at the requested
        size, even though the allocated buffer is rounded up past it.
        '''

        probe = largest_call_probe()
        probe.set_max_output_buffer(100)
        self.run_probe(probe)

        # After allocation max_output_buffer() reports the rounded-up size.
        # Without the cap, calls could reach half of it.
        self.assertGreater(probe.max_output_buffer(0), 200)
        self.assertGreater(probe.largest, 0)
        self.assertLessEqual(probe.largest, 100)
        self.assertEqual(probe.max_noutput_items(), 100)
        self.assertTrue(probe.is_set_max_noutput_items())


if __name__ == '__main__':
    gr_unittest.run(test_block_behavior)
