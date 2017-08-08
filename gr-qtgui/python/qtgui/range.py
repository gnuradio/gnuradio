#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from PyQt4 import Qt, QtCore, QtGui
import util

class Range(object):
    def __init__(self, minv, maxv, step, default, min_length):
        self.min = float(minv)
        self.max = float(maxv)
        self.step = float(step)
        self.default = float(default)
        self.min_length = min_length
        self.find_precision()
        self.find_nsteps()
        util.check_set_qss()

    def find_precision(self):
        # Get the decimal part of the step
        temp = str(float(self.step) - int(self.step))[2:]
        precision = len(temp) if temp is not '0' else 0
        precision = min(precision, 13)

        if precision == 0 and self.max < 100:
            self.precision = 1  # Always have a decimal in this case
        else:
            self.precision = (precision + 2) if precision > 0 else 0

    def find_nsteps(self):
        self.nsteps = (self.max + self.step - self.min)/self.step

    def demap_range(self, val):
        if val > self.max:
            val = self.max
        if val < self.min:
            val = self.min
        return ((val-self.min)/self.step)

    def map_range(self, val):
        if val > self.nsteps:
            val = self.max
        if val < 0:
            val = 0
        return (val*self.step+self.min)


class RangeWidget(QtGui.QWidget):
    def __init__(self, ranges, slot, label, style, rangeType=float):
        """ Creates the QT Range widget """
        QtGui.QWidget.__init__(self)

        self.range = ranges
        self.style = style

        # rangeType tells the block how to return the value as a standard
        self.rangeType = rangeType

        # Top-block function to call when any value changes
        # Some widgets call this directly when their value changes.
        # Others have intermediate functions to map the value into the right range.
        self.notifyChanged = slot

        layout = Qt.QHBoxLayout()
        label = Qt.QLabel(label)
        layout.addWidget(label)

        if style == "dial":
            self.d_widget = self.Dial(self, self.range, self.notifyChanged, rangeType)
        elif style == "slider":
            self.d_widget = self.Slider(self, self.range, self.notifyChanged, rangeType)
        elif style == "counter":
            # The counter widget can be directly wired to the notifyChanged slot
            self.d_widget = self.Counter(self, self.range, self.notifyChanged, rangeType)
        else:
            # The CounterSlider needs its own internal handlers before calling notifyChanged
            self.d_widget = self.CounterSlider(self, self.range, self.notifyChanged, rangeType)

        layout.addWidget(self.d_widget)
        self.setLayout(layout)

    class Dial(QtGui.QDial):
        """ Creates the range using a dial """
        def __init__(self, parent, ranges, slot, rangeType=float):
            QtGui.QDial.__init__(self, parent)

            self.rangeType = rangeType

            # Setup the dial
            self.setRange(0, ranges.nsteps-1)
            self.setSingleStep(1)
            self.setNotchesVisible(True)
            self.range = ranges

            # Round the initial value to the closest tick
            temp = int(round(ranges.demap_range(ranges.default), 0))
            self.setValue(temp)

            # Setup the slots
            self.valueChanged.connect(self.changed)
            self.notifyChanged = slot

        def changed(self, value):
            """ Handles maping the value to the right range before calling the slot. """
            val = self.range.map_range(value)
            self.notifyChanged(self.rangeType(val))

    class Slider(QtGui.QSlider):
        """ Creates the range using a slider """
        def __init__(self, parent, ranges, slot, rangeType=float):
            QtGui.QSlider.__init__(self, QtCore.Qt.Horizontal, parent)

            self.rangeType = rangeType

            # Setup the slider
            #self.setFocusPolicy(QtCore.Qt.NoFocus)
            self.setRange(0, ranges.nsteps - 1)
            self.setTickPosition(2)
            self.setSingleStep(1)
            self.range = ranges

            # Round the initial value to the closest tick
            temp = int(round(ranges.demap_range(ranges.default), 0))
            self.setValue(temp)

            if ranges.nsteps > ranges.min_length:
                interval = int(ranges.nsteps/ranges.min_length)
                self.setTickInterval(interval)
                self.setPageStep(interval)
            else:
                self.setTickInterval(1)
                self.setPageStep(1)

            # Setup the handler function
            self.valueChanged.connect(self.changed)
            self.notifyChanged = slot

        def changed(self, value):
            """ Handle the valueChanged signal and map the value into the correct range """
            val = self.range.map_range(value)
            self.notifyChanged(self.rangeType(val))

        def mousePressEvent(self, event):
            if((event.button() == QtCore.Qt.LeftButton)):
                new = self.minimum() + ((self.maximum()-self.minimum()) * event.x()) / self.width()
                self.setValue(new)
                event.accept()
            # Use repaint rather than calling the super mousePressEvent.
            # Calling super causes issue where slider jumps to wrong value.
            QtGui.QSlider.repaint(self)

        def mouseMoveEvent(self, event):
            new = self.minimum() + ((self.maximum()-self.minimum()) * event.x()) / self.width()
            self.setValue(new)
            event.accept()
            QtGui.QSlider.repaint(self)

    class Counter(QtGui.QDoubleSpinBox):
        """ Creates the range using a counter """
        def __init__(self, parent, ranges, slot, rangeType=float):
            QtGui.QDoubleSpinBox.__init__(self, parent)

            self.rangeType = rangeType

            # Setup the counter
            self.setDecimals(ranges.precision)
            self.setRange(ranges.min, ranges.max)
            self.setValue(ranges.default)
            self.setSingleStep(ranges.step)
            self.setKeyboardTracking(False)

            # The counter already handles floats and can be connected directly.
            self.valueChanged.connect(self.changed)
            self.notifyChanged = slot

        def changed(self, value):
            """ Handle the valueChanged signal by converting to the right type """
            self.notifyChanged(self.rangeType(value))

    class CounterSlider(QtGui.QWidget):
        """ Creates the range using a counter and slider """
        def __init__(self, parent, ranges, slot, rangeType=float):
            QtGui.QWidget.__init__(self, parent)

            self.rangeType = rangeType

            # Slot to call in the parent
            self.notifyChanged = slot

            self.slider = RangeWidget.Slider(parent, ranges, self.sliderChanged, rangeType)
            self.counter = RangeWidget.Counter(parent, ranges, self.counterChanged, rangeType)

            # Need another horizontal layout to wrap the other widgets.
            layout = Qt.QHBoxLayout()
            layout.addWidget(self.slider)
            layout.addWidget(self.counter)
            self.setLayout(layout)

            # Flag to ignore the slider event caused by a change to the counter.
            self.ignoreSlider = False
            self.range = ranges

        def sliderChanged(self, value):
            """ Handles changing the counter when the slider is updated """
            # If the counter was changed, ignore any of these events
            if not self.ignoreSlider:
                # Value is already float. Just set the counter
                self.counter.setValue(self.rangeType(value))
                self.notifyChanged(self.rangeType(value))
            self.ignoreSlider = False

        def counterChanged(self, value):
            """ Handles changing the slider when the counter is updated """
            # Get the current slider value and check to see if the new value changes it
            current = self.slider.value()
            new = int(round(self.range.demap_range(value), 0))

            # If it needs to change, ignore the slider event
            # Otherwise, the slider will cause the counter to round to the nearest tick
            if current != new:
                self.ignoreSlider = True
                self.slider.setValue(new)

            self.notifyChanged(self.rangeType(value))


if __name__ == "__main__":
    from PyQt4 import Qt
    import sys

    def valueChanged(frequency):
        print("Value updated - " + str(frequency))

    app = Qt.QApplication(sys.argv)
    widget = RangeWidget(Range(0, 100, 10, 1, 100), valueChanged, "Test", "counter_slider", int)

    widget.show()
    widget.setWindowTitle("Test Qt Range")
    app.exec_()

    widget = None
