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


class var_to_msg_pair(gr.sync_block):
    """
    This block has a callback that will emit a message pair with the updated variable
    value when called. This is useful for monitoring a GRC variable and emitting a message
    when it is changed.
    """

    def __init__(self, pairname):
        gr.sync_block.__init__(
            self, name="var_to_msg_pair", in_sig=None, out_sig=None)

        self.pairname = pairname

        self.message_port_register_out(pmt.intern("msgout"))

    def variable_changed(self, value):
        try:
            if type(value) == float:
                p = pmt.from_double(value)
            elif type(value) == int:
                p = pmt.from_long(value)
            elif type(value) == bool:
                p = pmt.from_bool(value)
            elif type(value) == str:
                p = pmt.intern(value)
            else:
                p = pmt.to_pmt(value)

            self.message_port_pub(pmt.intern("msgout"),
                                  pmt.cons(pmt.intern(self.pairname), p))

        except Exception as e:
            gr.log.error("Unable to convert " + repr(value) +
                         " to PDU, no message will be emitted (reason: %s)" % repr(e))

    def stop(self):
        return True
