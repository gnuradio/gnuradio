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


class Range(QtGui.QWidget):

    def __init__(self, min, max, step, default, style):
        QtGui.QWidget.__init__(self)

        self.min = min
        self.max = max
        self.step = step
        self.default = default

        if style == "dial":
            self.setupDial()
        elif style == "slider":
            self.setupSlider()
        elif style == "counter":
            self.setupCounter()
        elif style == "counter_slider":
            self.setupCounterSlider()


    def setupCounterSlider(self):
        """ Creates the range using a counter and slider """

        layout = Qt.QHBoxLayout()
        slider = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        slider.setFocusPolicy(QtCore.Qt.NoFocus)
        slider.setRange(self.min, self.max)
        slider.setPageStep(self.step)
        slider.setSingleStep(self.step)
        slider.setTracking(False)
        slider.setInvertedControls(True)
        layout.addWidget(slider)

        counter = QtGui.QDoubleSpinBox(self)
        counter.setRange(self.min, self.max)
        counter.setSingleStep(self.step)
        counter.setDecimals(0)
        layout.addWidget(counter)

        # Wire the events to each other
        counter.valueChanged.connect(slider.setValue)
        slider.valueChanged.connect(counter.setValue)

        self.setLayout(layout)

    def setupCounter(self):
        """ Creates the range using a counter """

        layout = Qt.QHBoxLayout()
        counter = QtGui.QDoubleSpinBox(self)
        counter.setRange(self.min, self.max)
        counter.setSingleStep(self.step)
        counter.setDecimals(0)
        layout.addWidget(counter)
        self.setLayout(layout)

    def setupSlider(self):
        """ Creates the range using a slider """

        layout = Qt.QHBoxLayout()
        slider = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        slider.setFocusPolicy(QtCore.Qt.NoFocus)
        slider.setRange(self.min, self.max)
        slider.setPageStep(self.step)
        slider.setSingleStep(self.step)
        slider.setTracking(False)
        slider.setInvertedControls(True)
        layout.addWidget(slider)
        self.setLayout(layout)

    def init_dial(self):
        layout = Qt.QHBoxLayout()
        dial = QtGui.QDial(self)
        dial.setRange(min, max)
        dial.setSingleStep(step)
        dial.setNotchesVisible(True)
        dial.setValue(init_value)
        layout.addWidget(dial)
        self.setLayout(layout)
