#!/usr/bin/env python
#
# Copyright 2006,2007,2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import weakref

from gnuradio import blocks, gr, gr_unittest
import pmt


class block_with_message_output(gr.basic_block):

    def __init__(self):
        gr.basic_block.__init__(self,
                                "block_with_message_output",
                                in_sig=None,
                                out_sig=None)
        self.message_port_register_out(pmt.intern("test"))


class block_with_message_input(gr.basic_block):

    def __init__(self):
        gr.basic_block.__init__(self,
                                "block_with_message_input",
                                in_sig=None,
                                out_sig=None)
        self.message_port_register_in(pmt.intern("test"))


class hier_block_with_message_output(gr.hier_block2):

    def __init__(self):
        gr.hier_block2.__init__(self,
                                "hier_block_with_message_output",
                                gr.io_signature(0, 0, 0),  # Input signature
                                gr.io_signature(0, 0, 0))  # Output signature
        self.message_port_register_hier_out("test")
        self.block = block_with_message_output()
        self.msg_connect(self.block, "test", self, "test")


class hier_block_with_message_input(gr.hier_block2):

    def __init__(self):
        gr.hier_block2.__init__(self,
                                "hier_block_with_message_output",
                                gr.io_signature(0, 0, 0),  # Input signature
                                gr.io_signature(0, 0, 0))  # Output signature
        self.message_port_register_hier_in("test")
        self.block = block_with_message_input()
        self.msg_connect(self, "test", self.block, "test")


class hier_block_with_message_inout(gr.hier_block2):

    def __init__(self):
        gr.hier_block2.__init__(self,
                                "hier_block_with_message_inout",
                                gr.io_signature(0, 0, 0),  # Input signature
                                gr.io_signature(0, 0, 0))  # Output signature
        self.message_port_register_hier_in("test")
        self.message_port_register_hier_out("test")
        self.input = block_with_message_input()
        self.msg_connect(self, "test", self.input, "test")
        self.output = block_with_message_output()
        self.msg_connect(self.output, "test", weakref.proxy(self), "test")


class test_hier_block2_message_connections(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def run_top_block(self):
        self.tb.start()
        self.tb.stop()
        self.tb.wait()

    def assert_has_subscription(self, sender, send_port, receiver,
                                receive_port):
        """assert that the given sender block has a subscription for the given
        receiver block on the appropriate send and receive ports

        :param sender: a block sptr to the message sender
        :param string send_port: the port messages are being sent on
        :param receiver: a block sptr to the message receiver
        :param string receive_port: the port messages are being received on
        """
        subs = sender.message_subscribers(pmt.intern(send_port))
        self.assertTrue(pmt.list_has(subs, pmt.cons(
            pmt.intern(receiver.to_basic_block().alias()),
            pmt.intern(receive_port))))

    def assert_has_num_subscriptions(self, block, port, number):
        """assert that the given block has the given number of subscriptions
        on the given port

        :param block: a block sptr
        :param string port: the port name
        :param number: the number of subscriptions expected
        """
        subs = block.message_subscribers(pmt.intern(port))
        self.assertEqual(pmt.length(subs), number)

    def test_hier_out_to_normal_in(self):
        message_debug = blocks.message_debug()
        hier = hier_block_with_message_output()

        self.tb.msg_connect(hier, "test", message_debug, "print")
        self.run_top_block()
        self.assert_has_num_subscriptions(hier.block, "test", 1)
        self.assert_has_num_subscriptions(hier, "test", 0)
        self.assert_has_subscription(
            hier.block, "test", message_debug, "print")
        self.tb.msg_disconnect(hier, "test", message_debug, "print")
        self.assert_has_num_subscriptions(hier.block, "test", 0)

    def test_normal_out_to_hier_in(self):
        b = block_with_message_output()
        hier = hier_block_with_message_input()

        self.tb.msg_connect(b, "test", hier, "test")
        self.run_top_block()
        self.assert_has_num_subscriptions(b, "test", 1)
        self.assert_has_subscription(b, "test", hier.block, "test")
        self.tb.msg_disconnect(b, "test", hier, "test")
        self.assert_has_num_subscriptions(b, "test", 0)

    def test_hier_out_to_hier_in(self):
        hier_out = hier_block_with_message_output()
        hier_in = hier_block_with_message_input()

        self.tb.msg_connect(hier_out, "test", hier_in, "test")
        self.run_top_block()
        self.assert_has_num_subscriptions(hier_out, "test", 0)
        self.assert_has_num_subscriptions(hier_out.block, "test", 1)
        self.assert_has_subscription(
            hier_out.block, "test", hier_in.block, "test")
        self.tb.msg_disconnect(hier_out, "test", hier_in, "test")
        self.assert_has_num_subscriptions(hier_out.block, "test", 0)

    def test_normal_in_to_hier_to_normal_out(self):
        hier = hier_block_with_message_inout()
        input = block_with_message_output()
        output = block_with_message_input()

        self.tb.msg_connect(input, "test", hier, "test")
        self.tb.msg_connect(hier, "test", output, "test")
        self.run_top_block()
        self.assert_has_num_subscriptions(input, "test", 1)
        self.assert_has_subscription(input, "test", hier.input, "test")
        self.assert_has_num_subscriptions(hier, "test", 0)
        self.assert_has_num_subscriptions(hier.output, "test", 1)
        self.assert_has_subscription(hier.output, "test", output, "test")
        self.tb.msg_disconnect(input, "test", hier, "test")
        self.tb.msg_disconnect(hier, "test", output, "test")
        self.assert_has_num_subscriptions(input, "test", 0)
        self.assert_has_num_subscriptions(hier.output, "test", 0)

    def test_multiple_connections(self):
        hier = hier_block_with_message_output()
        x = block_with_message_input()
        y = block_with_message_input()

        self.tb.msg_connect(hier, "test", x, "test")
        self.tb.msg_connect(hier, "test", y, "test")
        self.run_top_block()
        self.assert_has_num_subscriptions(hier, "test", 0)
        self.assert_has_num_subscriptions(hier.block, "test", 2)
        self.assert_has_subscription(hier.block, "test", x, "test")
        self.assert_has_subscription(hier.block, "test", y, "test")
        self.tb.msg_disconnect(hier, "test", y, "test")
        self.assert_has_num_subscriptions(hier, "test", 0)
        self.assert_has_num_subscriptions(hier.block, "test", 1)
        self.assert_has_subscription(hier.block, "test", x, "test")
        self.run_top_block()
        self.tb.msg_disconnect(hier, "test", x, "test")
        self.assert_has_num_subscriptions(hier.block, "test", 0)


if __name__ == '__main__':
    gr_unittest.run(test_hier_block2_message_connections)
