#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
import pmt


class meta_to_pair(gr.sync_block):
    """
    This block converts a metadata dictionary item to a pmt pair that is
    compatible with other blocks expecting a pair in.  You can specify
    which item in the incoming metadata to output as a pair and what
    the pair name is.
    """

    def __init__(self, incomingKeyName, outgoingPairName):
        gr.sync_block.__init__(self, name="meta_to_pair",
                               in_sig=None, out_sig=None)

        self.incomingKeyName = str(incomingKeyName)
        self.outgoingPairName = str(outgoingPairName)

        self.message_port_register_in(pmt.intern("inmeta"))
        self.set_msg_handler(pmt.intern("inmeta"), self.msg_handler)
        self.message_port_register_out(pmt.intern("outpair"))

    def msg_handler(self, msg):
        if not pmt.is_pair(msg):
            gr.log.warn("Incoming message is not a pair.  Only pairs are supported.  "
                        "No message generated.")
            return

        meta = pmt.to_python(pmt.car(msg))

        if not type(meta) is dict:
            gr.log.warn("Incoming message does not contain a dictionary.  "
                        "No message generated.")
            return

        if not self.incomingKeyName in meta:
            gr.log.warn("Incoming message dictionary does not contain key %s.  "
                        "No message generated." % self.incomingKeyName)
            return

        incomingVal = meta[self.incomingKeyName]

        new_pair = None

        try:
            new_pair = pmt.cons(pmt.intern(
                self.outgoingPairName), pmt.to_pmt(incomingVal))
        except Exception as e:
            gr.log.error("Cannot construct new message: %s" % str(e))
            return

        try:
            self.message_port_pub(pmt.intern("outpair"), new_pair)
        except Exception as e:
            gr.log.error("Cannot send message: %s" % str(e))
            gr.log.error("Incoming dictionary (%s):" % str(type(meta)))
            gr.log.error(str(meta))

    def stop(self):
        return True
