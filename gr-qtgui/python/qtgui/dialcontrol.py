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
from qtpy.QtWidgets import QFrame, QVBoxLayout, QLabel
from qtpy.QtCore import AlignCenter, QSize
from gnuradio import gr
import pmt


class LabeledDialControl(QFrame):
    def __init__(self, lbl='', parent=None, minimum=0, maximum=100, defaultvalue=0,
                 backgroundColor='default', changedCallback=None,
                 minsize=100, isFloat=False, scaleFactor=1, showvalue=False,
                 outputmsgname='value'):
        QFrame.__init__(self, parent)
        self.numberControl = DialControl(minimum, maximum, defaultvalue / scaleFactor, backgroundColor,
                                         self.valChanged, changedCallback, minsize)

        layout = QVBoxLayout()

        self.outputmsgname = outputmsgname
        self.showvalue = showvalue
        self.isFloat = isFloat
        self.scaleFactor = scaleFactor
        self.lbl = lbl
        self.lblcontrol = QLabel(lbl, self)
        self.lblcontrol.setAlignment(AlignCenter)

        if self.showvalue:
            textstr = self.buildTextStr(defaultvalue)
            self.lblcontrol.setText(textstr)

        if len or self.showvalue:
            self.hasLabel = True
            layout.addWidget(self.lblcontrol)
        else:
            self.hasLabel = False

        layout.addWidget(self.numberControl)

        layout.setAlignment(AlignCenter)
        self.setLayout(layout)
        self.show()

    def buildTextStr(self, new_value):
        textstr = ""
        if self.lbl:
            textstr = self.lbl + " - "

        if self.isFloat:
            textstr += "%.2f" % (new_value)
        else:
            textstr += str(new_value)

        return textstr

    def valChanged(self, new_value):
        if not self.showvalue:
            return

        if int(self.scaleFactor) != 1:
            new_value = new_value * self.scaleFactor

        textstr = self.buildTextStr(new_value)
        self.lblcontrol.setText(textstr)


class DialControl(QtWidgets.QDial):
    def __init__(self, minimum=0, maximum=100, defaultvalue=0, backgroundColor='default',
                 lablelCallback=None, changedCallback=None, minsize=100):
        QtWidgets.QDial.__init__(self)

        if backgroundColor != "default":
            self.setStyleSheet("background-color: " + backgroundColor + ";")

        self.minsize = minsize
        self.changedCallback = changedCallback
        self.lablelCallback = lablelCallback
        super().setMinimum(int(minimum))
        super().setMaximum(int(maximum))
        super().setValue(int(defaultvalue))
        super().valueChanged.connect(self.sliderMoved)

    def minimumSizeHint(self):
        return QSize(self.minsize, self.minsize)

    def sliderMoved(self):
        if self.changedCallback is not None:
            self.changedCallback(self.value())

        if self.lablelCallback is not None:
            self.lablelCallback(self.value())


class GrDialControl(gr.sync_block, LabeledDialControl):
    """
    This block creates a dial control. The control does control a
    variable which can be used for other items.  Leave the label
    blank to use the variable id as the label. The block also
    creates an optional message with the control value that
    can be used in message-based applications.

    Note: Dials only produce integer values, so the scale factor
    can be used with the min/max to adjust the output value to
    the desired range.  Think of the min/max as the increments,
    and the scale factor as the adjustment to get the values you want.
    """

    def __init__(self, lbl, parent, minimum, maximum, defaultvalue, backgroundColor='default',
                 varCallback=None, isFloat=False,
                 scaleFactor=1, minsize=100, showvalue=False, outputmsgname='value'):
        gr.sync_block.__init__(self, name="GrDialControl",
                               in_sig=None, out_sig=None)
        LabeledDialControl.__init__(self, lbl, parent, minimum, maximum, defaultvalue,
                                    backgroundColor, self.valueChanged, minsize, isFloat,
                                    scaleFactor, showvalue)

        self.outputmsgname = outputmsgname
        self.varCallback = varCallback
        self.scaleFactor = scaleFactor
        self.isFloat = isFloat
        self.message_port_register_out(pmt.intern("value"))

    def valueChanged(self, new_value):
        if int(self.scaleFactor) != 1:
            new_value = new_value * self.scaleFactor

        if self.varCallback is not None:
            self.varCallback(new_value)

        if self.isFloat:
            self.message_port_pub(pmt.intern("value"), pmt.cons(pmt.intern(self.outputmsgname),
                                                                pmt.from_double(new_value)))
        else:
            self.message_port_pub(pmt.intern("value"), pmt.cons(pmt.intern(self.outputmsgname),
                                                                pmt.from_long(new_value)))
