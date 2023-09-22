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
import pmt
import numpy
import time

# Simple block to generate messages


class message_generator(gr.sync_block):
    def __init__(self, msg_list, msg_interval):
        gr.sync_block.__init__(
            self,
            name="message generator",
            in_sig=[numpy.float32],
            out_sig=None
        )
        self.msg_list = msg_list
        self.msg_interval = msg_interval
        self.msg_ctr = 0
        self.message_port_register_out(pmt.intern('out_port'))

    def work(self, input_items, output_items):
        inLen = len(input_items[0])
        while self.msg_ctr < len(self.msg_list) and \
                (self.msg_ctr * self.msg_interval) < \
                (self.nitems_read(0) + inLen):
            self.message_port_pub(pmt.intern('out_port'),
                                  self.msg_list[self.msg_ctr])
            self.msg_ctr += 1
        return inLen

# Simple block to consume messages


class message_consumer(gr.sync_block):
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name="message consumer",
            in_sig=None,
            out_sig=None
        )
        self.msg_list = []
        self.message_port_register_in(pmt.intern('in_port'))
        self.set_msg_handler(pmt.intern('in_port'),
                             self.handle_msg)

    def handle_msg(self, msg):
        # Create a new PMT from long value and put in list
        self.msg_list.append(pmt.from_long(pmt.to_long(msg)))


class test_python_message_passing(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        num_msgs = 10
        msg_interval = 1000
        msg_list = []
        for i in range(num_msgs):
            msg_list.append(pmt.from_long(i))

        # Create vector source with dummy data to trigger messages
        src_data = []
        for i in range(num_msgs * msg_interval):
            src_data.append(float(i))
        src = blocks.vector_source_f(src_data, False)
        msg_gen = message_generator(msg_list, msg_interval)
        msg_cons = message_consumer()

        # Connect vector source to message gen
        self.tb.connect(src, msg_gen)

        # Connect message generator to message consumer
        self.tb.msg_connect(msg_gen, 'out_port', msg_cons, 'in_port')

        # Verify that the message port query functions work
        self.assertEqual(
            pmt.to_python(
                msg_gen.message_ports_out())[0],
            'out_port')
        self.assertEqual(
            'in_port' in pmt.to_python(
                msg_cons.message_ports_in()), True)

        # Run to verify message passing
        self.tb.run()

        # Verify that the message consumer got all the messages
        self.assertEqual(num_msgs, len(msg_cons.msg_list))
        for i in range(num_msgs):
            self.assertTrue(pmt.equal(msg_list[i], msg_cons.msg_list[i]))


if __name__ == '__main__':
    gr_unittest.run(test_python_message_passing)
