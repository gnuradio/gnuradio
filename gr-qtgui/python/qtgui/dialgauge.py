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

import sys
from qtpy.QtCore import Qt
from qtpy.QtWidgets import QFrame, QHBoxLayout, QVBoxLayout, QLabel
from qtpy.QtGui import QPainter, QColor, QPen, QFont, QFontMetricsF

from gnuradio import gr
import pmt


class LabeledDialGauge(QFrame):
    # Positions: 1 = above, 2=below, 3=left, 4=right
    def __init__(self, lbl='', barColor='blue', backgroundColor='white', fontColor='black',
                 minValue=0, maxValue=100, maxSize=80, position=1,
                 isFloat=False, showValue=False, fixedOrMin=True, parent=None):
        QFrame.__init__(self, parent)
        self.numberControl = DialGauge(barColor, backgroundColor, fontColor, minValue,
                                       maxValue, maxSize, isFloat, showValue, fixedOrMin, parent)

        if position < 3:
            layout = QVBoxLayout()
        else:
            layout = QHBoxLayout()

        self.lbl = lbl
        self.showvalue = showValue
        self.isFloat = isFloat

        self.lblcontrol = QLabel(lbl, self)
        self.lblcontrol.setAlignment(Qt.AlignCenter)

        # For whatever reason, the progressbar doesn't show the number in the bar if it's
        # vertical, only if it's horizontal
        if len:
            self.lblcontrol.setText(lbl)

        if fontColor != 'default':
            self.lblcontrol.setStyleSheet(
                "QLabel { color : " + fontColor + "; }")

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

        layout.setAlignment(Qt.AlignCenter | Qt.AlignVCenter)
        self.setLayout(layout)

        self.show()

    def setValue(self, new_value):
        self.numberControl.setValue(new_value)


class DialGauge(QFrame):
    def __init__(self, barColor='blue', backgroundColor='white', fontColor='black',
                 minValue=0, maxValue=100, maxSize=80,
                 isFloat=False, showValue=False, fixedOrMin=True, parent=None):
        QFrame.__init__(self, parent)

        self.maxSize = maxSize
        super().setMinimumSize(maxSize, maxSize)
        if fixedOrMin:
            super().setMaximumSize(maxSize, maxSize)

        self.backgroundColor = backgroundColor
        self.barColor = barColor
        self.fontColor = fontColor
        self.isFloat = isFloat
        self.showValue = showValue

        self.value = minValue

        self.minValue = minValue
        self.maxValue = maxValue

        self.textfont = QFont(self.font())
        self.textfont.setPixelSize(16)
        self.metrics = QFontMetricsF(self.textfont)

        self.startAngle = 0.0
        self.endAngle = 360.0
        self.degScaler = 16.0  # The span angle must be specified in 1/16 of a degree units
        self.penWidth = max(int(0.1 * maxSize), 6)
        self.halfPenWidth = int(self.penWidth / 2)

    def getValue(self):
        if self.isFloat:
            return float(self.value)
        else:
            return int(self.value)

    def setValue(self, new_value):
        if new_value > self.maxValue:
            new_value = self.maxValue
        elif new_value < self.minValue:
            new_value = self.minValue

        self.value = float(new_value)

        super().update()

    def paintEvent(self, event):
        super().paintEvent(event)

        size = self.size()

        percentRange = float(self.value - self.minValue) / \
            float(self.maxValue - self.minValue)
        endAngle = self.startAngle + \
            round(percentRange * float(self.endAngle - self.startAngle), 0)

        # Now convert angles to 1/16 scale
        startAngle = int(round(self.startAngle * self.degScaler, 0))
        endAngle = int(round(endAngle * self.degScaler, 0))

        rect = QtCore.QRect(self.halfPenWidth, self.halfPenWidth, size.width() - self.penWidth,
                            size.height() - self.penWidth)

        # Set up the painting canvass
        painter = QPainter()
        painter.begin(self)
        painter.setRenderHint(QPainter.Antialiasing)

        if self.showValue:
            painter.setFont(self.textfont)
            painter.setPen(QPen(QColor(self.fontColor)))

            if self.isFloat:
                printText = "%.2f" % self.value
            else:
                printText = str(int(self.value))

            painter.drawText(int(size.width() / 2 - self.metrics.width(printText) / 2), size.height() // 2,
                             printText)

        painter.save()
        painter.translate(self.width(), 0)
        painter.rotate(90.0)

        # First draw complete circle
        painter.setPen(QPen(QColor(self.backgroundColor), self.penWidth))
        painter.drawArc(rect, startAngle, int(self.endAngle * self.degScaler))
        # First draw complete circle
        painter.setPen(QPen(QColor(self.barColor), self.penWidth))
        painter.drawArc(rect, startAngle, -endAngle)
        painter.setPen(QPen(QColor('darkgray'), 2))
        painter.drawEllipse(1, 1, rect.width() + self.penWidth -
                            2, rect.width() + self.penWidth - 2)
        painter.drawEllipse(1 + self.penWidth, 1 + self.penWidth, rect.width() - self.penWidth - 2,
                            rect.width() - self.penWidth - 2)
        painter.restore()

        painter.end()


class GrDialGauge(gr.sync_block, LabeledDialGauge):
    """
    This block creates a dial-style gauge. The value can be set
    either with a variable or an input message.
    """

    def __init__(self, lbl='', barColor='blue', backgroundColor='white', fontColor='black',
                 minValue=0, maxValue=100, maxSize=80,
                 position=1, isFloat=False, showValue=False, fixedOrMin=True, parent=None):
        gr.sync_block.__init__(self, name="DialGauge",
                               in_sig=None, out_sig=None)
        LabeledDialGauge.__init__(self, lbl, barColor, backgroundColor, fontColor, minValue,
                                  maxValue, maxSize, position, isFloat, showValue, fixedOrMin,
                                  parent)
        self.lbl = lbl

        if minValue > maxValue:
            gr.log.error("Min value is greater than max value.")
            sys.exit(1)

        self.message_port_register_in(pmt.intern("value"))
        self.set_msg_handler(pmt.intern("value"), self.msgHandler)

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))

            if type(new_val) is float or type(new_val) is int:
                super().setValue(new_val)
            else:
                gr.log.error("Value received was not an int or a float. "
                             "Received %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def setValue(self, new_value):
        super().setValue(new_value)
