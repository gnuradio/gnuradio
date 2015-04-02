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

class Range(object):
    def __init__(self, minv, maxv, step, default):
        self.min = float(minv)
        self.max = float(maxv)
        self.step = float(step)
        self.default = float(default)
        self.find_precision()

    def find_precision(self):
        temp = str(float(self.step)-int(self.step))[2:]
        if len(temp) > 15:
          self.precision = 15
        else:
          self.precision = len(temp)

class RangeWidget(QtGui.QWidget):
    def __init__(self, ranges, slot, label, style):
        """ Creates the QT Range widget """
        QtGui.QWidget.__init__(self)

        self.range = ranges

        layout = Qt.QHBoxLayout()
        label = Qt.QLabel(label)
        layout.addWidget(label)

        if style == "dial":
            layout.addWidget(self.Dial(self, self.range, slot))
        elif style == "slider":
            layout.addWidget(self.Slider(self, self.range, slot))
        elif style == "counter":
            layout.addWidget(self.Counter(self, self.range, slot))
        elif style == "counter_slider":
            layout.addWidget(self.CounterSlider(self, self.range, slot))
        else:
            layout.addWidget(self.CounterSlider(self, self.range, slot))

        self.setLayout(layout)

    class Dial(QtGui.QDial):
        """ Creates the range using a dial """
        def __init__(self, parent, ranges, slot):
            QtGui.QDial.__init__(self, parent)
            self.setRange(ranges.min, ranges.max)
            self.setSingleStep(ranges.step)
            self.setNotchesVisible(True)
            self.setNotchTarget(ranges.step)
            self.setValue(ranges.default)
            self.valueChanged.connect(slot)

    class Slider(QtGui.QSlider):
        """ Creates the range using a slider """
        def __init__(self, parent, ranges, slot):
            QtGui.QSlider.__init__(self, QtCore.Qt.Horizontal, parent)
            self.setFocusPolicy(QtCore.Qt.NoFocus)
            self.setRange(ranges.min, ranges.max)
            self.setValue(ranges.default)
            self.setPageStep(ranges.step)
            self.setSingleStep(ranges.step)
            self.setTickInterval(ranges.step)
            self.setTracking(False)
            self.setInvertedControls(True)
            self.valueChanged.connect(slot)

    class Counter(QtGui.QDoubleSpinBox):
        """ Creates the range using a counter """
        def __init__(self, parent, ranges, slot):
            QtGui.QDoubleSpinBox.__init__(self, parent)
            self.setRange(ranges.min, ranges.max)
            self.setValue(ranges.default)
            self.setSingleStep(ranges.step)
            self.setDecimals(ranges.precision)
            self.valueChanged.connect(slot)

    class CounterSlider(QtGui.QWidget):
        """ Creates the range using a counter and slider """
        def __init__(self, parent, ranges, slot):
            QtGui.QWidget.__init__(self, parent)

            # Need another horizontal layout
            layout = Qt.QHBoxLayout()

            # Create a slider with the top-level widget as the parent
            slider = RangeWidget.Slider(parent,ranges,slot)
            layout.addWidget(slider)

            # Setup the counter
            counter = RangeWidget.Counter(parent,ranges,slot)
            layout.addWidget(counter)

            # Wire the events to each other
            counter.valueChanged.connect(slider.setValue)
            slider.valueChanged.connect(counter.setValue)
            counter.valueChanged.connect(slot)
            slider.valueChanged.connect(slot)

            self.setLayout(layout)


