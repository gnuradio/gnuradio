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

class msg_pair_to_var(gr.sync_block):
    """
    This block will take an input message pair and allow you to set a flowgraph variable
    via setter callback. If the second element the pair is a compound PMT object or not
    of the datatype expected by the flowgraph the behavior of the flowgraph may be
    unpredictable.
    """
    def __init__(self, callback):
        gr.sync_block.__init__(self, name="msg_pair_to_var", in_sig=None, out_sig=None)

        self.callback = callback

        self.message_port_register_in(pmt.intern("inpair"))
        self.set_msg_handler(pmt.intern("inpair"), self.msg_handler)

    def msg_handler(self, msg):
        if not pmt.is_pair(msg) or pmt.is_dict(msg) or pmt.is_pdu(msg):
            gr.log.warn("Input message %s is not a simple pair, dropping" % repr(msg))
            return

        new_val = pmt.to_python(pmt.cdr(msg))
        try:
            self.callback(new_val)
        except Exception as e:
            gr.log.error("Error when calling " + repr(self.callback.name()) + " with "
                         + repr(new_val) + " (reason: %s)"  % repr(e))

    def stop(self):
        return True
