#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 Ettus Research, a National Instruments Brand.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from PyQt5 import Qt
from gnuradio import gr
import pmt


class ReplayMsgPushButton(gr.sync_block, Qt.QPushButton):
    """
    This block creates a variable push button that creates a message
    when clicked. The message will be formatted as a dictionary to pass
    to the RFNoC Replay block
    """

    def __init__(self, lbl, relBackColor, relFontColor,
                 command, port, offset=-1, size=-1, time=-1, repeat=False):
        gr.sync_block.__init__(self, name="ReplayMsgPushButton",
                               in_sig=None, out_sig=None)
        Qt.QPushButton.__init__(self, lbl)

        self.lbl = lbl

        self.replayDict = {'command': command}
        self.replayDict['port'] = port
        self.replayDict['repeat'] = repeat
        # If the user does not specify a command offset, size, or time,
        # don't add to the command. The properties will be used for
        # offset and size, and the command will happen immediately.
        if offset != -1:
            self.replayDict['offset'] = offset
        if size != -1:
            self.replayDict['size'] = size
        if time != -1:
            self.replayDict['time'] = time

        styleStr = ""
        if relBackColor != 'default':
            styleStr = "background-color: " + relBackColor + "; "

        if relFontColor:
            styleStr += "color: " + relFontColor + "; "

        self.setStyleSheet(styleStr)

        self.clicked[bool].connect(self.onBtnClicked)

        self.message_port_register_out(pmt.intern("pressed"))

    def set_command(self, command):
        self.replayDict['command'] = command

    def set_port(self, port):
        self.replayDict['port'] = port

    def set_size(self, size):
        self.replayDict['size'] = size

    def set_offset(self, offset):
        self.replayDict['offset'] = offset

    def set_time(self, time):
        self.replayDict['time'] = time

    def set_repeat(self, repeat):
        self.replayDict['repeat'] = repeat

    def onBtnClicked(self, pressed):
        self.message_port_pub(pmt.intern("pressed"), pmt.to_pmt(self.replayDict))
