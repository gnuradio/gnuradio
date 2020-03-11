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
    This block will monitor a variable, and when it changes, generate a message.
    """
    def __init__(self, pairname):
        gr.sync_block.__init__(self, name="var_to_msg_pair", in_sig=None, out_sig=None)

        self.pairname = pairname

        self.message_port_register_out(pmt.intern("msgout"))

    def variable_changed(self, value):
        if type(value) == float:
            p = pmt.from_float(value)
        elif type(value) == int:
            p = pmt.from_long(value)
        elif type(value) == bool:
            p = pmt.from_bool(value)
        else:
            p = pmt.intern(value)

        self.message_port_pub(pmt.intern("msgout"), pmt.cons(pmt.intern(self.pairname), p))

    def stop(self):
        return True
