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
    def __init__(self, min, max, step, default):
        self.min = min
        self.max = max
        self.step = step
        self.default = default

class RangeWidget(QtGui.QWidget):
    def __init__(self, range, slot, label, style):
        """ Creates the QT Range widget """
        QtGui.QWidget.__init__(self)

        self.range = range

        layout = Qt.QHBoxLayout()
        label = Qt.QLabel(label)
        layout.addWidget(label)

        if style == "dial":
            layout.addWidget(self.Dial(self, range, slot))
        elif style == "slider":
            layout.addWidget(self.Slider(self, range, slot))
        elif style == "counter":
            layout.addWidget(self.Counter(self, range, slot))
        elif style == "counter_slider":
            layout.addWidget(self.CounterSlider(self, range, slot))
        else:
            layout.addWidget(self.CounterSlider(self, range, slot))

        self.setLayout(layout)

    class Dial(QtGui.QDial):
        """ Creates the range using a dial """
        def __init__(self, parent, range, slot):
            QtGui.QDial.__init__(self, parent)
            self.setRange(range.min, range.max)
            self.setSingleStep(range.step)
            self.setNotchesVisible(True)
            self.setValue(range.default)
            self.valueChanged.connect(slot)

    class Slider(QtGui.QSlider):
        """ Creates the range using a slider """
        def __init__(self, parent, range, slot):
            QtGui.QSlider.__init__(self, QtCore.Qt.Horizontal, parent)
            self.setFocusPolicy(QtCore.Qt.NoFocus)
            self.setRange(range.min, range.max)
            self.setValue(range.default)
            self.setPageStep(range.step)
            self.setSingleStep(range.step)
            self.setTracking(False)
            self.setInvertedControls(True)
            self.valueChanged.connect(slot)

    class Counter(QtGui.QDoubleSpinBox):
        """ Creates the range using a counter """
        def __init__(self, parent, range, slot):
            QtGui.QDoubleSpinBox.__init__(self, parent)
            self.setRange(range.min, range.max)
            self.setValue(range.default)
            self.setSingleStep(range.step)
            self.setDecimals(0)
            self.valueChanged.connect(slot)

    class CounterSlider(QtGui.QWidget):
        """ Creates the range using a counter and slider """
        def __init__(self, parent, range, slot):
            QtGui.QWidget.__init__(self, parent)

            # Need another horizontal layout
            layout = Qt.QHBoxLayout()

            # Create a slider with the top-level widget as the parent
            slider = QtGui.QSlider(QtCore.Qt.Horizontal, self)
            slider.setFocusPolicy(QtCore.Qt.NoFocus)
            slider.setRange(range.min, range.max)
            slider.setValue(range.default)
            slider.setPageStep(range.step)
            slider.setSingleStep(range.step)
            slider.setTracking(False)
            slider.setInvertedControls(True)
            layout.addWidget(slider)

            # Setup the counter
            counter = QtGui.QDoubleSpinBox(self)
            counter.setRange(range.min, range.max)
            counter.setValue(range.default)
            counter.setSingleStep(range.step)
            counter.setDecimals(0)
            layout.addWidget(counter)

            # Wire the events to each other
            counter.valueChanged.connect(slider.setValue)
            slider.valueChanged.connect(counter.setValue)
            counter.valueChanged.connect(slot)
            slider.valueChanged.connect(slot)

            self.setLayout(layout)
