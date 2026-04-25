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

from PyQt5.QtWidgets import QFrame, QVBoxLayout, QLabel
from PyQt5.QtGui import QPainter, QPixmap, QFont, QFontMetrics, QBrush, QColor
from PyQt5.QtCore import Qt, QSize
from PyQt5 import QtCore
from PyQt5.QtCore import Qt as Qtc
from PyQt5.QtCore import pyqtSignal

from gnuradio import gr
import pmt

# -------------- Support Classes ---------------------------------


class LabeledDigitalNumberControl(QFrame):
    def __init__(
        self,
        lbl="",
        min_freq_hz=0,
        max_freq_hz=6000000000,
        parent=None,
        thousands_separator=",",
        background_color="black",
        fontColor="white",
        click_callback=None,
    ):
        QFrame.__init__(self, parent)
        self.numberControl = DigitalNumberControl(
            min_freq_hz,
            max_freq_hz,
            self,
            thousands_separator,
            background_color,
            fontColor,
            click_callback,
        )

        layout = QVBoxLayout()

        self.lbl = QLabel(lbl, self)
        if len:
            self.hasLabel = True
            layout.addWidget(self.lbl)
        else:
            self.hasLabel = False

        layout.addWidget(self.numberControl)
        layout.setAlignment(Qtc.AlignCenter | Qtc.AlignVCenter)
        self.setLayout(layout)
        self.show()

    def minimumSizeHint(self):
        if self.hasLabel:
            return QSize(self.numberControl.minimumWidth() + 10, 100)
        else:
            return QSize(self.numberControl.minimumWidth() + 10, 50)

    def setReadOnly(self, b_read_only):
        self.numberControl.setReadOnly(b_read_only)

    def setMinFrequency(self, min_freq_hz):
        self.numberControl.setMinFrequency(min_freq_hz)

    def setMaxFrequency(self, max_freq_hz):
        self.numberControl.setMaxFrequency(max_freq_hz)

    def setFrequencyRange(self, min_freq_hz, max_freq_hz):
        self.numberControl.setFrequencyRange(min_freq_hz, max_freq_hz)

    def getMinFrequency(self):
        return self.numberControl.getMinFrequency()

    def getMaxFrequency(self):
        return self.numberControl.getMaxFrequency()

    def setFrequency(self, new_freq):
        self.numberControl.setFrequency(new_freq)

    def getFrequency(self):
        return self.numberControl.getFrequency()


class DigitalNumberControl(QFrame):
    # Notifies to avoid thread conflicts on paints
    updateInt = pyqtSignal(int)
    updateFloat = pyqtSignal(float)

    def __init__(
        self,
        min_freq_hz=0,
        max_freq_hz=6000000000,
        parent=None,
        thousands_separator=",",
        background_color="black",
        fontColor="white",
        click_callback=None,
    ):
        QFrame.__init__(self, parent)

        self.updateInt.connect(self.onUpdateInt)
        self.updateFloat.connect(self.onUpdateFloat)

        self.min_freq = int(min_freq_hz)
        self.max_freq = int(max_freq_hz)
        self.numDigitsInFreq = len(str(max_freq_hz))
        

        self.thousands_separator = thousands_separator
        self.click_callback = click_callback

        self.read_only = False

        self.setColors(QColor(background_color), QColor(fontColor))
        self.numberFont = QFont("Arial", 12, QFont.Normal)

        self.cur_freq = min_freq_hz

        # Determine what our width minimum is
        teststr = ""
        for i in range(0, self.numDigitsInFreq):
            teststr += "0"

        fm = QFontMetrics(self.numberFont)
        if len(self.thousands_separator) > 0:
            # The -1 makes sure we don't count an extra for 123,456,789.
            # Answer should be 2 not 3.
            numgroups = int(float(self.numDigitsInFreq - 1) / 3.0)
            if numgroups > 0:
                for i in range(0, numgroups):
                    teststr += self.thousands_separator

            textstr = teststr
        else:
            textstr = teststr

        width = fm.width(textstr)

        self.minwidth = width

        if self.minwidth < 410:
            self.minwidth = 410

        self.setMaximumHeight(70)
        self.setMinimumWidth(self.minwidth)
        # Show the control
        self.show()

    def _update_width_from_digits(self):
        # Size from the configured range so separators/layout track max digits.
        teststr = "0" * self.numDigitsInFreq

        fm = QFontMetrics(self.numberFont)
        if len(self.thousands_separator) > 0:
            # The -1 makes sure we don't count an extra for 123,456,789.
            # Answer should be 2 not 3.
            numgroups = int(float(self.numDigitsInFreq - 1) / 3.0)
            if numgroups > 0:
                teststr += self.thousands_separator * numgroups

        width = fm.width(teststr)

        self.minwidth = width
        if self.minwidth < 410:
            self.minwidth = 410

        self.setMinimumWidth(self.minwidth)

    def _clamp_current_frequency(self):
        if self.cur_freq < self.min_freq:
            self.cur_freq = self.min_freq
        elif self.cur_freq > self.max_freq:
            self.cur_freq = self.max_freq

    def minimumSizeHint(self):
        return QSize(self.minwidth, 50)

    def setReadOnly(self, b_read_only):
        self.read_only = b_read_only

    def setMinFrequency(self, min_freq_hz):
        min_freq_hz = int(min_freq_hz)
        if min_freq_hz > self.max_freq:
            gr.log.error("Min frequency cannot exceed max frequency")
            return

        self.min_freq = min_freq_hz
        self._clamp_current_frequency()
        self.update()

    def setMaxFrequency(self, max_freq_hz):
        max_freq_hz = int(max_freq_hz)
        if max_freq_hz < self.min_freq:
            gr.log.error("Max frequency cannot be less than min frequency")
            return

        self.max_freq = max_freq_hz
        self.numDigitsInFreq = len(str(self.max_freq))
        self._update_width_from_digits()
        self._clamp_current_frequency()
        self.update()

    def setFrequencyRange(self, min_freq_hz, max_freq_hz):
        min_freq_hz = int(min_freq_hz)
        max_freq_hz = int(max_freq_hz)
        if min_freq_hz > max_freq_hz:
            gr.log.error("Min frequency cannot exceed max frequency")
            return

        self.min_freq = min_freq_hz
        self.max_freq = max_freq_hz
        self.numDigitsInFreq = len(str(self.max_freq))
        self._update_width_from_digits()
        self._clamp_current_frequency()
        self.update()

    def getMinFrequency(self):
        return self.min_freq

    def getMaxFrequency(self):
        return self.max_freq

    def mousePressEvent(self, event):
        super(DigitalNumberControl, self).mousePressEvent(event)
        self.offset = event.pos()

        if self.read_only:
            gr.log.trace("click received but read-only.  Not changing frequency.")
            return

        fm = QFontMetrics(self.numberFont)

        if len(self.thousands_separator) > 0:
            if self.thousands_separator != ".":
                textstr = format(self.getFrequency(), self.thousands_separator)
            else:
                textstr = format(self.getFrequency(), ",")
                textstr = textstr.replace(",", ".")
        else:
            textstr = str(self.getFrequency())

        width = fm.width(textstr)

        # So we know:
        # - the width of the text
        # - The mouse click position relative to 0
        #   (pos relative to string start will be size().width() - 2 - pos.x)

        clickpos = self.size().width() - 2 - self.offset.x()

        found_number = False
        clicked_thousands = False
        for i in range(1, len(textstr) + 1):
            width = fm.width(textstr[-i:])
            charstr = textstr[-i:]
            widthchar = fm.width(charstr[0])
            if clickpos >= (width - widthchar) and clickpos <= width:
                clicked_char = i - 1

                clicked_num_index = clicked_char

                found_number = True

                if len(self.thousands_separator) > 0:
                    if charstr[0] != self.thousands_separator:
                        numSeps = charstr.count(self.thousands_separator)
                        clicked_num_index -= numSeps
                        gr.log.trace(f"clicked number: {clicked_num_index}")
                    else:
                        clicked_thousands = True
                        gr.log.trace("clicked thousands separator")
                else:
                    gr.log.trace("clicked number: " + str(clicked_char))

                # Remember y=0 is at the top so this is reversed
                clicked_up = False
                if self.offset.y() > self.size().height() / 2:
                    gr.log.trace("clicked down")
                else:
                    gr.log.trace("clicked up")
                    clicked_up = True

                if not clicked_thousands:
                    cur_freq = self.getFrequency()
                    increment = pow(10, clicked_num_index)
                    if clicked_up:
                        cur_freq += increment
                    else:
                        cur_freq -= increment

                    # Cannot call setFrequency to emit.
                    # Change must happen now for paint event when clicked.
                    self.setFrequencyNow(cur_freq)

                    if self.click_callback is not None:
                        self.click_callback(self.getFrequency())
                break

        if (not found_number) and (not clicked_thousands):
            # See if we clicked in the high area, if so, increment there.
            clicked_up = False
            if self.offset.y() > self.size().height() / 2:
                gr.log.trace("clicked down in the high area")
            else:
                gr.log.trace("clicked up in the high area")
                clicked_up = True

            textstr = str(self.getFrequency())
            numNumbers = len(textstr)
            increment = pow(10, numNumbers)
            cur_freq = self.getFrequency()
            if clicked_up:
                cur_freq += increment
            else:
                cur_freq -= increment

            # Cannot call setFrequency to emit.  Change must happen now for
            # paint event when clicked.
            self.setFrequencyNow(cur_freq)

            if self.click_callback is not None:
                gr.log.trace("Calling self.click_callback")

                self.click_callback(self.getFrequency())
            else:
                gr.log.trace("self.click_callback is None.  Not calling callback.")

    def setColors(self, background, fontColor):
        self.background_color = background
        self.fontColor = fontColor

    def reverseString(self, astring):
        astring = astring[::-1]
        return astring

    def onUpdateInt(self, new_freq):
        if (new_freq >= self.min_freq) and (new_freq <= self.max_freq):
            self.cur_freq = int(new_freq)

        self.update()

    def onUpdateFloat(self, new_freq):
        if (new_freq >= self.min_freq) and (new_freq <= self.max_freq):
            self.cur_freq = int(new_freq)

        self.update()

    def setFrequencyNow(self, new_freq):
        # This setFrequency differs from setFrequency() in that it
        # it updates the display immediately rather than emitting
        # to the message queue as required during msg handling
        if (new_freq >= self.min_freq) and (new_freq <= self.max_freq):
            self.cur_freq = int(new_freq)
            self.update()

    def setFrequency(self, new_freq):
        if type(new_freq) == int:
            self.updateInt.emit(new_freq)
        else:
            self.updateFloat.emit(new_freq)

    def getFrequency(self):
        return self.cur_freq

    def resizeEvent(self, event):
        self.pxMap = QPixmap(self.size())
        self.pxMap.fill(self.background_color)

        self.update()

    def paintEvent(self, event):
        super().paintEvent(event)

        painter = QPainter(self)

        size = self.size()
        brush = QBrush()
        brush.setColor(self.background_color)
        brush.setStyle(Qt.SolidPattern)
        rect = QtCore.QRect(2, 2, size.width() - 4, size.height() - 4)
        painter.fillRect(rect, brush)

        self.numberFont.setPixelSize(int(0.9 * size.height()))
        painter.setFont(self.numberFont)
        painter.setPen(self.fontColor)
        rect = event.rect()

        if len(self.thousands_separator) > 0:
            if self.thousands_separator != ".":
                textstr = format(self.getFrequency(), self.thousands_separator)
            else:
                textstr = format(self.getFrequency(), ",")
                textstr = textstr.replace(",", ".")
        else:
            textstr = str(self.getFrequency())

        rect = QtCore.QRect(0, 0, size.width() - 4, size.height())

        painter.drawText(rect, Qt.AlignRight + Qt.AlignVCenter, textstr)


class MsgDigitalNumberControl(gr.sync_block, LabeledDigitalNumberControl):
    """
    GNU Radio Block Class
    """

    def __init__(
        self,
        lbl="",
        min_freq_hz=0,
        max_freq_hz=6000000000,
        parent=None,
        thousands_separator=",",
        background_color="black",
        fontColor="white",
        var_callback=None,
        outputmsgname="freq",
    ):
        gr.sync_block.__init__(
            self, name="MsgDigitalNumberControl", in_sig=None, out_sig=None
        )
        LabeledDigitalNumberControl.__init__(
            self,
            lbl,
            min_freq_hz,
            max_freq_hz,
            parent,
            thousands_separator,
            background_color,
            fontColor,
            self.click_callback,
        )

        self.var_callback = var_callback
        self.outputmsgname = outputmsgname

        self.message_port_register_in(pmt.intern("valuein"))
        self.set_msg_handler(pmt.intern("valuein"), self.msgHandler)
        self.message_port_register_in(pmt.intern("min"))
        self.set_msg_handler(pmt.intern("min"), self.handle_min_msg)
        self.message_port_register_in(pmt.intern("max"))
        self.set_msg_handler(pmt.intern("max"), self.handle_max_msg)
        self.message_port_register_out(pmt.intern("valueout"))

    def _msg_to_numeric_value(self, msg):
        if pmt.is_real(msg):
            return pmt.to_double(msg)

        return pmt.to_double(pmt.cdr(msg))

    def msgHandler(self, msg):
        try:
            new_val = pmt.to_python(pmt.cdr(msg))

            if type(new_val) == float or type(new_val) == int:
                self.call_var_callback(new_val)

                self.setValue(new_val)
            else:
                gr.log.error(
                    "Value received was not an int or a float. %s" % str(type(new_val))
                )

        except Exception as e:
            gr.log.error("Error with message conversion: %s" % str(e))

    def handle_min_msg(self, msg):
        try:
            self.set_min_freq(self._msg_to_numeric_value(msg))
        except Exception as e:
            gr.log.error("Error with min message conversion: %s" % str(e))

    def handle_max_msg(self, msg):
        try:
            self.set_max_freq(self._msg_to_numeric_value(msg))
        except Exception as e:
            gr.log.error("Error with max message conversion: %s" % str(e))

    def call_var_callback(self, new_value):
        if self.var_callback is not None:
            if type(self.var_callback) is float:
                self.var_callback = float(new_value)
            else:
                self.var_callback(float(new_value))

    def click_callback(self, new_value):
        self.call_var_callback(new_value)

        self.message_port_pub(
            pmt.intern("valueout"),
            pmt.cons(pmt.intern(self.outputmsgname), pmt.from_double(float(new_value))),
        )

    def setValue(self, new_val):
        self.setFrequency(new_val)

        self.message_port_pub(
            pmt.intern("valueout"),
            pmt.cons(pmt.intern(self.outputmsgname), pmt.from_double(float(new_val))),
        )

    def getValue(self):
        self.getFrequency()

    def setReadOnly(self, b_read_only):
        super().setReadOnly(b_read_only)

    def setMinFrequency(self, min_freq_hz):
        prev_value = self.getFrequency()
        super().setMinFrequency(min_freq_hz)
        new_value = self.getFrequency()
        if new_value != prev_value:
            self.call_var_callback(new_value)
            self.setValue(new_value)

    def set_min_freq(self, min_freq_hz):
        self.setMinFrequency(min_freq_hz)

    def setMaxFrequency(self, max_freq_hz):
        prev_value = self.getFrequency()
        super().setMaxFrequency(max_freq_hz)
        new_value = self.getFrequency()
        if new_value != prev_value:
            self.call_var_callback(new_value)
            self.setValue(new_value)

    def set_max_freq(self, max_freq_hz):
        self.setMaxFrequency(max_freq_hz)

    def setFrequencyRange(self, min_freq_hz, max_freq_hz):
        super().setFrequencyRange(min_freq_hz, max_freq_hz)
