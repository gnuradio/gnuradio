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

from qtpy.QtWidgets import QFrame, QHBoxLayout, QVBoxLayout, QLabel
from qtpy.QtGui import QPainter, QBrush, QColor, QPen, QFontMetricsF, QRadialGradient
from qtpy.QtCore import Qt as Qtc, QPointF

from gnuradio import gr
import pmt


class LabeledLEDIndicator(QFrame):
    # Positions: 1 = above, 2=below, 3=left, 4=right
    def __init__(self, lbl='', onColor='green', offColor='red', initialState=False, maxSize=80,
                 position=1, alignment=1, valignment=1, parent=None):
        QFrame.__init__(self, parent)
        self.numberControl = LEDIndicator(
            onColor, offColor, initialState, maxSize, parent)

        if position < 3:
            layout = QVBoxLayout()
        else:
            layout = QHBoxLayout()

        if not lbl:
            lbl = " "

        self.lbl = lbl
        self.lblcontrol = QLabel(lbl, self)
        self.lblcontrol.setAlignment(Qtc.AlignCenter)

        # add top or left
        if len:
            if position == 1 or position == 3:
                layout.addWidget(self.lblcontrol)
        else:
            self.hasLabel = False

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

        if len:
            textfont = self.lblcontrol.font()
            metrics = QFontMetricsF(textfont)

            maxWidth = int(max((maxSize + 30), (maxSize + metrics.width(lbl) + 4)))
            maxHeight = int(max((maxSize + 35), (maxSize + metrics.height() + 2)))
            self.setMinimumSize(maxWidth, maxHeight)
        else:
            self.setMinimumSize(maxSize + 2, maxSize + 2)

        self.show()

    def setState(self, on_off):
        self.numberControl.setState(on_off)


class LEDIndicator(QFrame):
    def __init__(self, onColor='green', offColor='red', initialState=False, maxSize=80,
                 parent=None):
        QFrame.__init__(self, parent)

        self.maxSize = maxSize
        self.curState = initialState
        self.on_color = QColor(onColor)
        self.off_color = QColor(offColor)

        self.setMinimumSize(maxSize, maxSize)
        self.setMaximumSize(maxSize, maxSize)

    def setState(self, on_off):
        self.curState = on_off
        super().update()

    def paintEvent(self, event):
        super().paintEvent(event)

        painter = QPainter(self)

        size = self.size()
        brush = QBrush()

        smallest_dim = size.width()
        if smallest_dim > size.height():
            smallest_dim = size.height()

        smallest_dim = smallest_dim / 2
        smallest_dim -= 2

        center_x = int(size.width() / 2)
        center_y = int(size.height() / 2)
        centerpoint = QPointF(center_x, center_y)

        radius = smallest_dim

        painter.setPen(QPen(QColor('lightgray'), 0))
        brush.setStyle(Qtc.SolidPattern)

        radial = QRadialGradient(center_x, center_y / 2, radius)
        radial.setColorAt(0, Qtc.white)
        radial.setColorAt(0.8, Qtc.darkGray)
        painter.setBrush(QBrush(radial))
        painter.drawEllipse(centerpoint, radius, radius)

        # Draw the colored center
        radial = QRadialGradient(center_x, center_y / 2, radius)
        radial.setColorAt(0, Qtc.white)

        if self.curState:
            radial.setColorAt(.7, self.on_color)
            brush.setColor(self.on_color)
            painter.setPen(QPen(self.on_color, 0))
        else:
            radial.setColorAt(.7, self.off_color)
            brush.setColor(self.off_color)
            painter.setPen(QPen(self.off_color, 0))

        brush.setStyle(Qtc.SolidPattern)
        painter.setBrush(QBrush(radial))
        if smallest_dim <= 30:
            radius = radius - 3
        elif smallest_dim <= 60:
            radius = radius - 4
        elif smallest_dim <= 100:
            radius = radius - 5
        elif smallest_dim <= 200:
            radius = radius - 6
        elif smallest_dim <= 300:
            radius = radius - 7
        else:
            radius = radius - 9
        painter.drawEllipse(centerpoint, radius, radius)


class GrLEDIndicator(gr.sync_block, LabeledLEDIndicator):
    """
    This block makes a basic LED indicator
    """

    def __init__(self, lbl='', onColor='green', offColor='red', initialState=False,
                 maxSize=80, position=1, alignment=1, valignment=1, parent=None):
        gr.sync_block.__init__(self, name="LEDIndicator",
                               in_sig=None, out_sig=None)
        LabeledLEDIndicator.__init__(self, lbl, onColor, offColor, initialState,
                                     maxSize, position, alignment, valignment, parent)
        self.lbl = lbl

        self.message_port_register_in(pmt.intern("state"))
        self.set_msg_handler(pmt.intern("state"), self.msgHandler)

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))

            if type(new_val) is bool or type(new_val) == int:
                if type(new_val) is bool:
                    super().setState(new_val)
                else:
                    if new_val == 1:
                        super().setState(True)
                    else:
                        super().setState(False)
            else:
                gr.log.error(
                    "Value received was not an int or a bool: %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def setState(self, on_off):
        super().setState(on_off)
