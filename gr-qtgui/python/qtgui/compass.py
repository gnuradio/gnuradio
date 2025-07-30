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

import time
import numpy
from gnuradio import gr
import pmt

from qtpy.QtCore import Signal, Property, Qt, QPoint
from qtpy.QtWidgets import QFrame, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QSizePolicy
from qtpy.QtGui import QPainter, QPalette, QFont, QFontMetricsF, QPen, QPolygon, QColor, QBrush

NeedleFull = 1
NeedleIndicator = 0
NeedleMirrored = 2


class LabeledCompass(QFrame):
    def __init__(self, lbl, min_size, update_time, setDebug=False,
                 needleType=NeedleFull, position=1, backgroundColor='default'):
        # Positions: 1 = above, 2=below, 3=left, 4=right
        QFrame.__init__(self)
        self.numberControl = Compass(min_size, update_time, setDebug,
                                     needleType, position, backgroundColor)

        if position < 3:
            layout = QVBoxLayout()
        else:
            layout = QHBoxLayout()

        self.lbl = lbl
        self.lblcontrol = QLabel(lbl, self)
        self.lblcontrol.setAlignment(Qt.AlignCenter)

        # add top or left
        if lbl:
            if position == 1 or position == 3:
                layout.addWidget(self.lblcontrol)
        else:
            self.hasLabel = False

        layout.addWidget(self.numberControl)

        # Add bottom or right
        if lbl:
            if position == 2 or position == 4:
                layout.addWidget(self.lblcontrol)

        layout.setAlignment(Qt.AlignCenter | Qt.AlignVCenter)
        self.setLayout(layout)

        if lbl:
            self.setMinimumSize(min_size + 30, min_size + 35)
        else:
            self.setMinimumSize(min_size, min_size)

        self.show()

    def change_angle(self, angle):
        self.numberControl.change_angle(angle)

    def setColors(self, backgroundColor='default', needleTip='red', needleBody='black',
                  scaleColor='black'):
        self.numberControl.setColors(
            backgroundColor, needleTip, needleBody, scaleColor)


class Compass(QWidget):
    angleChanged = Signal(float)

    def __init__(self, min_size, update_time, setDebug=False, needleType=NeedleFull,
                 position=1, backgroundColor='default'):
        QWidget.__init__(self, None)

        # Set parameters
        self.debug = setDebug
        self.needleType = needleType
        self.update_period = update_time
        self.last = time.time()
        self.next_angle = 0

        self._angle = 0.0
        self._margins = 2
        self._pointText = {0: "0", 45: "45", 90: "90", 135: "135", 180: "180",
                           225: "225", 270: "270", 315: "315"}

        self.setMinimumSize(min_size, min_size)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

        self.backgroundColor = backgroundColor
        self.needleTipColor = 'red'
        self.needleBodyColor = 'black'
        self.scaleColor = 'black'

    def setColors(self, backgroundColor='default', needleTipColor='red', needleBodyColor='black',
                  scaleColor='black'):
        self.backgroundColor = backgroundColor
        self.needleTipColor = needleTipColor
        self.needleBodyColor = needleBodyColor
        self.scaleColor = scaleColor

        super().update()

    def paintEvent(self, event):
        painter = QPainter()
        painter.begin(self)
        painter.setRenderHint(QPainter.Antialiasing)

        if self.backgroundColor == 'default':
            painter.fillRect(
                event.rect(), self.palette().brush(QPalette.Window))
        else:
            size = self.size()
            center_x = size.width() / 2
            diameter = size.height()
            brush = QBrush(QColor(self.backgroundColor), Qt.SolidPattern)
            painter.setBrush(brush)
            painter.setPen(QPen(QColor(self.scaleColor), 2))
            painter.setRenderHint(QPainter.Antialiasing)
            painter.drawEllipse(int(center_x - diameter / 2 + 1),
                                1, diameter - 4, diameter - 4)

        self.drawMarkings(painter)
        self.drawNeedle(painter)

        painter.end()

    def drawMarkings(self, painter):
        painter.save()
        painter.translate(self.width() / 2, self.height() / 2)
        scale = min((self.width() - self._margins) / 120.0,
                    (self.height() - self._margins) / 120.0)
        painter.scale(scale, scale)

        font = QFont(self.font())
        font.setPixelSize(8)
        metrics = QFontMetricsF(font)

        painter.setFont(font)
        painter.setPen(QPen(QColor(self.scaleColor)))
        tickInterval = 5
        i = 0
        while i < 360:

            if i % 45 == 0:
                painter.drawLine(0, -40, 0, -50)
                painter.drawText(int(-metrics.width(
                    self._pointText[i]) / 2.0), -52, self._pointText[i])
            else:
                painter.drawLine(0, -45, 0, -50)

            painter.rotate(tickInterval)
            i += tickInterval

        painter.restore()

    def drawNeedle(self, painter):
        painter.save()
        # Set up painter
        painter.translate(self.width() / 2, self.height() / 2)
        scale = min((self.width() - self._margins) / 120.0,
                    (self.height() - self._margins) / 120.0)
        painter.scale(scale, scale)
        painter.setPen(QPen(Qt.NoPen))

        # Rotate surface for painting
        intAngle = int(round(self._angle))
        painter.rotate(intAngle)

        # Draw the full black needle first if needed
        if self.needleType == NeedleFull:
            needleTailBrush = self.palette().brush(QPalette.Shadow)
            needleTailColor = QColor(self.needleBodyColor)
            needleTailBrush.setColor(needleTailColor)
            painter.setBrush(needleTailBrush)

            painter.drawPolygon(QPolygon([QPoint(-6, 0), QPoint(0, -45), QPoint(6, 0),
                                          QPoint(0, 45), QPoint(-6, 0)]))

        # Now draw the red tip (on top of the black needle)
        needleTipBrush = self.palette().brush(QPalette.Highlight)
        needleTipColor = QColor(self.needleTipColor)
        needleTipBrush.setColor(needleTipColor)
        painter.setBrush(needleTipBrush)

        # First QPoint is the center bottom apex of the needle
        painter.drawPolygon(QPolygon([QPoint(-3, -24), QPoint(0, -45), QPoint(3, -23),
                                      QPoint(0, -30), QPoint(-3, -23)]))

        if self.needleType == NeedleMirrored:
            # Rotate
            # Need to account for the initial rotation to see how much more to rotate it.
            if (intAngle == 90 or intAngle == -90 or intAngle == 270):
                mirrorRotation = 180
            else:
                mirrorRotation = 180 - intAngle - intAngle
            painter.rotate(mirrorRotation)

            # Paint shadowed indicator
            needleTipBrush = self.palette().brush(QPalette.Highlight)
            needleTipColor = Qt.gray
            needleTipBrush.setColor(needleTipColor)
            painter.setBrush(needleTipBrush)

            painter.drawPolygon(
                QPolygon([QPoint(-3, -25), QPoint(0, -45), QPoint(3, -25),
                          QPoint(0, -30), QPoint(-3, -25)])
            )

        painter.restore()

    def angle(self):
        return self._angle

    def change_angle(self, angle):
        if angle != self._angle:
            if self.debug:
                gr.log.trace(f"Compass angle: {angle}")

            if angle < 0.0:
                angle = 360.0 + angle  # Angle will already be negative

            self._angle = angle
            self.angleChanged.emit(angle)
            self.update()

    angle = Property(float, angle, change_angle)


class GrCompass(gr.sync_block, LabeledCompass):
    """
    This block takes angle in degrees as input and displays it on a compass.
    Three different needle formats are available, Full, indicator only,
    and mirrored (mirrored is useful for direction-finding where an
    ambiguity exists in front/back detection angle).
    """

    def __init__(self, title, min_size, update_time, setDebug=False, needleType=NeedleFull,
                 usemsg=False, position=1, backgroundColor='default'):
        if usemsg:
            gr.sync_block.__init__(
                self, name="QTCompass", in_sig=[], out_sig=[])
        else:
            gr.sync_block.__init__(self, name="QTCompass", in_sig=[
                                   numpy.float32], out_sig=[])

        LabeledCompass.__init__(self, title, min_size, update_time, setDebug, needleType,
                                position, backgroundColor)

        self.last = time.time()
        self.update_period = update_time
        self.useMsg = usemsg

        self.next_angle = 0.0

        self.message_port_register_in(pmt.intern("angle"))
        self.set_msg_handler(pmt.intern("angle"), self.msgHandler)

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))

            if type(new_val) is float or type(new_val) is int:
                super().change_angle(float(new_val))
            else:
                gr.log.error(
                    "Value received was not an int or a float: %s" % str(type(new_val)))

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def setColors(self, backgroundColor='default', needleTip='red', needleBody='black',
                  scaleColor='black'):
        super().setColors(backgroundColor, needleTip, needleBody, scaleColor)

    def work(self, input_items, output_items):
        if self.useMsg:
            return len(input_items[0])

        # Average inputs
        self.next_angle = numpy.mean(input_items[0])

        if (time.time() - self.last) > self.update_period:
            self.last = time.time()
            super().change_angle(self.next_angle)

        # Consume all inputs
        return len(input_items[0])
