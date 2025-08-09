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

from gnuradio import gr
import pmt

from qtpy.QtCore import Qt as Qtc
from qtpy.QtCore import QRect
from qtpy.QtWidgets import QFrame, QHBoxLayout, QVBoxLayout, QLabel
from qtpy.QtGui import QPainter, QBrush, QColor, QPen, QFontMetricsF


class LabeledToggleSwitch(QFrame):
    # Positions: 1 = above, 2=below, 3=left, 4=right
    def __init__(self, lbl='', onColor='green', offColor='red', initialState=False,
                 maxSize=50, position=1, parent=None, callback=None, alignment=1, valignment=1):
        QFrame.__init__(self, parent)
        self.numberControl = ToggleSwitch(onColor, offColor, initialState, maxSize,
                                          parent, callback)

        if position < 3:
            layout = QVBoxLayout()
        else:
            layout = QHBoxLayout()

        self.lbl = lbl
        self.lblcontrol = QLabel(lbl, self)

        if position == 3:  # left of switch
            self.lblcontrol.setAlignment(Qtc.AlignRight)
        elif position == 4:  # right of switch
            self.lblcontrol.setAlignment(Qtc.AlignLeft)
        else:
            # Above or below
            self.lblcontrol.setAlignment(Qtc.AlignCenter)

        # add top or left
        if len:
            if position == 1 or position == 3:
                layout.addWidget(self.lblcontrol)

        layout.addWidget(self.numberControl)

        # Add bottom or right
        if len:
            if position == 2 or position == 4:
                layout.addWidget(self.lblcontrol)

        if alignment == 1:
            halign = Qtc.AlignCenter
        elif alignment == 2:
            halign = Qtc.AlignLeft
        else:
            halign = Qtc.AlignRight

        if valignment == 1:
            valign = Qtc.AlignVCenter
        elif valignment == 2:
            valign = Qtc.AlignTop
        else:
            valign = Qtc.AlignBottom

        layout.setAlignment(halign | valign)

        self.setLayout(layout)

        textfont = self.lblcontrol.font()
        metrics = QFontMetricsF(textfont)

        maxWidth = max((maxSize + 4), (maxSize * 2 + metrics.width(lbl)))
        maxHeight = max((maxSize // 2 + 4),
                        (maxSize // 2 + metrics.height() + 2))

        self.setMinimumSize(int(maxWidth), int(maxHeight))

        self.show()

    def setState(self, on_off):
        self.numberControl.setState(on_off)


class ToggleSwitch(QFrame):
    def __init__(self, onColor='green', offColor='red', initialState=False, maxSize=50,
                 parent=None, callback=None):
        QFrame.__init__(self, parent)

        self.maxSize = maxSize
        self.curState = initialState
        self.onColor = QColor(onColor)
        self.offColor = QColor(offColor)
        self.callback = callback
        self.setMinimumSize(maxSize, maxSize // 2)
        self.setMaximumSize(maxSize, maxSize // 2)

    def setState(self, on_off):
        self.curState = on_off
        if self.callback is not None:
            self.callback(on_off)

        super().update()

    def paintEvent(self, event):
        super().paintEvent(event)

        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        size = self.size()
        brush = QBrush()

        center_x = size.width() // 2

        if self.curState:
            brush.setColor(self.onColor)
            painter.setPen(QPen(self.onColor, 0))
        else:
            brush.setColor(self.offColor)
            painter.setPen(QPen(self.offColor, 0))

        brush.setStyle(Qtc.SolidPattern)
        painter.setBrush(brush)

        # Draw the switch background
        centerRect = QRect(size.width() // 4, 0,
                           size.width() // 2 - 4, size.height())
        painter.drawRect(centerRect)
        painter.drawEllipse(0, 0, size.height(), size.height())
        painter.drawEllipse(size.width() // 2, 0, size.height(), size.height())

        # Draw the switch itself
        brush.setColor(QColor('white'))
        painter.setBrush(brush)
        painter.setPen(QPen(QColor('white'), 0))
        if self.curState:
            painter.drawEllipse(
                center_x + 2, 2, size.height() - 4, size.height() - 4)
        else:
            painter.drawEllipse(2, 2, size.height() - 4, size.height() - 4)

    def mousePressEvent(self, event):
        if event.x() <= self.size().width() / 2:
            self.setState(False)
        else:
            self.setState(True)

        super().update()


class GrToggleSwitch(gr.sync_block, LabeledToggleSwitch):
    """
    This block creates a modern toggle switch. The variable will take
    on one value or the other as set in the dialog. This button will
    also produce a state message matching the set values.
    """

    def __init__(self, callback, lbl, pressedReleasedDict, initialState=False,
                 onColor='green', offColor='silver', position=3, maxSize=50,
                 alignment=1, valignment=1, parent=None, outputmsgname='value'):
        gr.sync_block.__init__(self, name="ToggleSwitch",
                               in_sig=None, out_sig=None)
        LabeledToggleSwitch.__init__(self, lbl, onColor, offColor, initialState,
                                     maxSize, position, parent, self.notifyUpdate,
                                     alignment, valignment)

        self.outputmsgname = outputmsgname
        self.pressReleasedDict = pressedReleasedDict
        self.callback = callback
        self.message_port_register_out(pmt.intern("state"))

    def notifyUpdate(self, new_val):
        if self.callback is not None:
            if new_val:
                self.callback(self.pressReleasedDict['Pressed'])
            else:
                self.callback(self.pressReleasedDict['Released'])

        if new_val:
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
