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

from qtpy import QtWidgets
from qtpy.QtCore import AlignTop, AlignLeft, AlignBottom, AlignRight, AlignCenter, AlignVCenter
from qtpy.QtWidgets import QFrame, QVBoxLayout

from gnuradio import gr
import pmt


class CheckBoxEx(QtWidgets.QCheckBox):
    def __init__(self, lbl, callback=None):
        QtWidgets.QCheckBox.__init__(self)
        self.setText(lbl)
        self.callback = callback

        self.stateChanged.connect(self.onToggleClicked)

    def onToggleClicked(self):
        if self.callback is not None:
            self.callback(super().isChecked())


class MsgCheckBox(gr.sync_block, QFrame):
    """
   This block creates a variable checkbox. Leave the label blank to
   use the variable id as the label. A checkbox selects between
   two values of similar type, but will stay depressed until
   clicked again. The variable will take on one value or the other
   depending on whether the button is pressed or released.

    This control will also produce a state message matching the
    set values.
    """

    def __init__(self, callback, lbl, pressedReleasedDict, initPressed, alignment,
                 valignment, outputmsgname='value'):
        gr.sync_block.__init__(self, name="MsgCheckBox",
                               in_sig=None, out_sig=None)
        QFrame.__init__(self)

        self.outputmsgname = outputmsgname
        self.chkBox = CheckBoxEx(lbl, self.onToggleClicked)

        layout = QVBoxLayout()

        layout.addWidget(self.chkBox)

        if alignment == 1:
            halign = AlignCenter
        elif alignment == 2:
            halign = AlignLeft
        else:
            halign = AlignRight

        if valignment == 1:
            valign = AlignVCenter
        elif valignment == 2:
            valign = AlignTop
        else:
            valign = AlignBottom

        layout.setAlignment(halign | valign)
        self.setLayout(layout)

        self.callback = callback
        self.pressReleasedDict = pressedReleasedDict

        self.message_port_register_out(pmt.intern("state"))

        if initPressed:
            self.chkBox.setChecked(True)

        self.show()

    def onToggleClicked(self, checked):
        if self.chkBox.isChecked():
            self.callback(self.pressReleasedDict['Pressed'])

            if type(self.pressReleasedDict['Pressed']) is bool:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_bool(self.pressReleasedDict['Pressed'])))
            elif type(self.pressReleasedDict['Pressed']) is int:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_long(self.pressReleasedDict['Pressed'])))
            elif type(self.pressReleasedDict['Pressed']) is float:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_double(self.pressReleasedDict['Pressed'])))
            else:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.intern(self.pressReleasedDict['Pressed'])))
        else:
            self.callback(self.pressReleasedDict['Released'])

            if type(self.pressReleasedDict['Released']) is bool:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_bool(self.pressReleasedDict['Released'])))
            elif type(self.pressReleasedDict['Released']) is int:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_long(self.pressReleasedDict['Released'])))
            elif type(self.pressReleasedDict['Released']) is float:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.from_double(self.pressReleasedDict['Released'])))
            else:
                self.message_port_pub(pmt.intern("state"),
                                      pmt.cons(pmt.intern(self.outputmsgname),
                                               pmt.intern(self.pressReleasedDict['Released'])))
