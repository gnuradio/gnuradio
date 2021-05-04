#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from PyQt5 import Qt
from gnuradio import gr
import pmt

class MsgPushButton(gr.sync_block, Qt.QPushButton):
    """
    This block creates a variable push button that creates a message
    when clicked. Leave the label blank to use the variable id as
    the label. You can define both the output message pmt name as
    well as the value and value type.
    """
    def __init__(self, lbl, msgName, msgValue, relBackColor, relFontColor):
        gr.sync_block.__init__(self, name="MsgPushButton", in_sig=None, out_sig=None)
        Qt.QPushButton.__init__(self, lbl)

        self.lbl = lbl
        self.msgName = msgName
        self.msgValue = msgValue

        styleStr = ""
        if relBackColor != 'default':
            styleStr = "background-color: " + relBackColor + "; "

        if relFontColor:
            styleStr += "color: " + relFontColor + "; "

        self.setStyleSheet(styleStr)

        self.clicked[bool].connect(self.onBtnClicked)

        self.message_port_register_out(pmt.intern("pressed"))

    def onBtnClicked(self, pressed):
        if type(self.msgValue) == int:
            self.message_port_pub(pmt.intern("pressed"),
                pmt.cons(pmt.intern(self.msgName), pmt.from_long(self.msgValue)))
        elif type(self.msgValue) == float:
            self.message_port_pub(pmt.intern("pressed"),
                pmt.cons(pmt.intern(self.msgName), pmt.from_double(self.msgValue)))
        elif type(self.msgValue) == str:
            self.message_port_pub(pmt.intern("pressed"),
                pmt.cons(pmt.intern(self.msgName), pmt.intern(self.msgValue)))
        elif type(self.msgValue) == bool:
            self.message_port_pub(pmt.intern("pressed"),
                pmt.cons(pmt.intern(self.msgName), pmt.from_bool(self.msgValue)))
