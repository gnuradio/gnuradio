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

from threading import Lock
import sys

from qtpy.QtCore import Signal, Qt
from qtpy.QtWidgets import QFrame, QHBoxLayout, QVBoxLayout, QLabel, QProgressBar
from qtpy.QtGui import QColor, QPalette

from gnuradio import gr
import pmt


class LabeledLevelGauge(QFrame):
    # Positions: 1 = above, 2=below, 3=left, 4=right
    def __init__(self, lbl='', barColor='blue', backgroundColor='white', fontColor='black',
                 minValue=0, maxValue=100, maxSize=80, position=1, isVertical=True,
                 isFloat=False, scaleFactor=1, showValue=False, parent=None):
        QFrame.__init__(self, parent)
        self.numberControl = LevelGauge(barColor, backgroundColor, minValue, maxValue,
                                        maxSize, isVertical, isFloat, scaleFactor, showValue,
                                        parent)

        if position < 3:
            layout = QVBoxLayout()
        else:
            layout = QHBoxLayout()

        self.lbl = lbl
        self.showvalue = showValue
        self.isFloat = isFloat
        self.isVertical = isVertical
        self.scaleFactor = scaleFactor

        self.lblcontrol = QLabel(lbl, self)
        self.lblcontrol.setAlignment(Qt.AlignCenter)

        # For whatever reason, the progressbar doesn't show the number in the bar if it's
        # vertical, only if it's horizontal
        if self.showvalue and (isFloat or self.isVertical):
            textstr = self.buildTextStr(minValue / self.scaleFactor)
            self.lblcontrol.setText(textstr)

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

    def buildTextStr(self, new_value):
        textstr = ""
        if len(self.lbl) > 0:
            textstr = self.lbl + " - "

        if self.isFloat:
            textstr += "%.2f" % (new_value)
        else:
            textstr += str(new_value)

        return textstr

    def valChanged(self, new_value):
        if not self.showvalue:
            return

        if self.isFloat or self.isVertical:
            if self.lbl:
                textstr = self.buildTextStr(new_value)
                self.lblcontrol.setText(textstr)

    def setValue(self, new_value):
        self.valChanged(new_value)

        if int(self.scaleFactor) != 1:
            new_value = int(new_value * self.scaleFactor)

        self.numberControl.setValue(new_value)


class LevelGauge(QProgressBar):
    # Notifies to avoid thread conflicts on paints
    updateInt = Signal(int)
    updateFloat = Signal(float)

    def __init__(self, barColor='blue', backgroundColor='white', minValue=0, maxValue=100,
                 maxSize=80, isVertical=True, isFloat=False, scaleFactor=1, showValue=False,
                 parent=None):
        super().__init__(parent)

        self.updateInt.connect(self.onUpdateInt)
        self.updateFloat.connect(self.onUpdateFloat)

        self.lock = Lock()

        self.maxSize = maxSize

        p = super().palette()

        if backgroundColor != 'default':
            p.setColor(QPalette.Base, QColor(backgroundColor))

        if barColor != 'default':
            p.setColor(QPalette.Highlight, QColor(barColor))

        if backgroundColor != 'default' or barColor != 'default':
            super().setPalette(p)

        if (not isFloat) and showValue:
            super().setFormat("%v")  # This shows the number in the bar itself.
            super().setTextVisible(True)
        else:
            super().setTextVisible(False)

        super().setMinimum(minValue)
        super().setMaximum(maxValue)

        if isVertical:
            super().setOrientation(Qt.Orientation.Vertical)
        else:
            super().setOrientation(Qt.Orientation.Horizontal)

    def onUpdateInt(self, new_value):
        if new_value > super().maximum():
            new_value = super().maximum()
        elif new_value < super().minimum():
            new_value = super().minimum()

        self.lock.acquire()
        super().setValue(new_value)
        self.lock.release()

    def onUpdateFloat(self, new_value):
        if new_value > super().maximum():
            new_value = super().maximum()
        elif new_value < super().minimum():
            new_value = super().minimum()

        self.lock.acquire()
        super().setValue(new_value)
        self.lock.release()

    def setValue(self, new_value):
        if type(new_value) is int:
            self.updateInt.emit(new_value)
        else:
            self.updateFloat.emit(new_value)


class GrLevelGauge(gr.sync_block, LabeledLevelGauge):
    """
    This block creates a level gauge. The value can be set either
    with a variable or an input message.

    NOTE: This control has some quirks due to the fact that
    QProgressBar only accepts integers.  If you want to work with
    floats, you have to use the scaleFactor to map incoming values
    to the specified min/max range.  For instance if the min/max
    are 0-100 but your incoming values are 0.0-1.0, you will need
    to set a scalefactor of 100.
    """

    def __init__(self, lbl='', barColor='blue', backgroundColor='white', fontColor='black',
                 minValue=0, maxValue=100, maxSize=80, isVertical=True, position=1,
                 isFloat=False, scaleFactor=1, showValue=False, parent=None):
        gr.sync_block.__init__(self, name="LevelGauge",
                               in_sig=None, out_sig=None)
        LabeledLevelGauge.__init__(self, lbl, barColor, backgroundColor, fontColor, minValue,
                                   maxValue, maxSize, position, isVertical, isFloat,
                                   scaleFactor, showValue, parent)
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
                gr.log.error(
                    "Value received was not an int or a float: %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def setValue(self, new_value):
        super().setValue(new_value)
