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

class ToggleButton(gr.sync_block, Qt.QPushButton):
    """
    This block creates a variable toggle button. Leave the label
    blank to use the variable id as the label. A toggle button
    selects between two values of similar type, but will stay
    depressed until clicked again. The variable will take on one
    value or the other depending on whether the button is pressed
    or released. This button will also produce a state message
    matching the set values.
    """
    def __init__(self, callback, lbl, pressedReleasedDict, initPressed, outputmsgname='value'):
        gr.sync_block.__init__(self, name="ToggleButton", in_sig=None, out_sig=None)
        Qt.QPushButton.__init__(self, lbl)
        self.setCheckable(True)
        self.lbl = lbl
        self.callback = callback
        self.pressReleasedDict = pressedReleasedDict

        self.outputmsgname = outputmsgname
        self.relBackColor = 'default'
        self.relFontColor = 'default'
        self.pressBackColor = 'default'
        self.pressFontColor = 'default'

        self.message_port_register_out(pmt.intern("state"))

        if initPressed:
            self.setChecked(True)
            self.state = 1
        else:
            self.state = 0

        self.clicked[bool].connect(self.onToggleClicked)

    def setColors(self, relBackColor, relFontColor, pressBackColor, pressFontColor):
        self.relBackColor = relBackColor
        self.relFontColor = relFontColor
        self.pressBackColor = pressBackColor
        self.pressFontColor = pressFontColor

        self.setColor()

    def setColor(self):
        if self.state:
            styleStr = ""
            if self.pressBackColor != 'default':
                styleStr = "background-color: " + self.pressBackColor + "; "

            if self.pressFontColor:
                styleStr += "color: " + self.pressFontColor + "; "

            self.setStyleSheet(styleStr)
        else:
            styleStr = ""
            if self.relBackColor != 'default':
                styleStr = "background-color: " + self.relBackColor + "; "

            if self.relFontColor:
                styleStr += "color: " + self.relFontColor + "; "

            self.setStyleSheet(styleStr)

    def onToggleClicked(self, pressed):
        if pressed:
            self.state = 1
            self.callback(self.pressReleasedDict['Pressed'])
        else:
            self.state = 0
            self.callback(self.pressReleasedDict['Released'])

        self.setColor()

        if pressed:
            if type(self.pressReleasedDict['Pressed']) == bool:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_bool(self.pressReleasedDict['Pressed'])))
            elif type(self.pressReleasedDict['Pressed']) == int:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_long(self.pressReleasedDict['Pressed'])))
            elif type(self.pressReleasedDict['Pressed']) == float:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_double(self.pressReleasedDict['Pressed'])))
            else:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.intern(self.pressReleasedDict['Pressed'])))
        else:
            if type(self.pressReleasedDict['Released']) == bool:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_bool(self.pressReleasedDict['Released'])))
            elif type(self.pressReleasedDict['Released']) == int:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_long(self.pressReleasedDict['Released'])))
            elif type(self.pressReleasedDict['Released']) == float:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.from_double(self.pressReleasedDict['Released'])))
            else:
                self.message_port_pub(pmt.intern("state"),
                                    pmt.cons(pmt.intern(self.outputmsgname),
                                    pmt.intern(self.pressReleasedDict['Released'])))
