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
import numpy

class Range(object):
    def __init__(self, minv, maxv, step, default, min_length):
        self.min = float(minv)
        self.max = float(maxv)
        self.step = float(step)
        self.default = float(default)
        self.min_length = min_length
        self.find_precision()
        self.find_nsteps()

    def find_precision(self):
        temp = str(float(self.step)-int(self.step))[2:]
        if len(temp) > 13:
          self.precision = 15
        else:
          self.precision = len(temp)+2

    def find_nsteps(self):
        temp = numpy.arange(self.min,self.max+self.step,self.step)
        self.ds_steps = len(temp)
        self.ds_vals  = (numpy.linspace(self.min,self.max,num=self.ds_steps)).tolist()

class RangeWidget(QtGui.QWidget):
    def __init__(self, ranges, slot, label, style):
        """ Creates the QT Range widget """
        QtGui.QWidget.__init__(self)

        self.range = ranges
        self.slot = slot
        self.style = style

        layout = Qt.QHBoxLayout()
        label = Qt.QLabel(label)
        layout.addWidget(label)

        if style == "dial":
            self.d_widget = self.Dial(self, self.range, self.ds_modified_slot)
        elif style == "slider":
            self.d_widget = self.Slider(self, self.range, self.ds_modified_slot)
        elif style == "counter":
            self.d_widget = self.Counter(self, self.range, self.c_modified_slot)
        elif style == "counter_slider":
            self.d_widget = self.CounterSlider(self, self.range, self.ds_modified_slot, self.c_modified_slot)
        else:
            self.d_widget = self.CounterSlider(self, self.range, self.ds_modified_slot, self.c_modified_slot)

        layout.addWidget(self.d_widget)
        self.setLayout(layout)

    def ds_modified_slot(self,val):
        self.slot(self.range.ds_vals[val])
        if self.style == "counter_slider":
          self.d_widget.set_counter(self.range.ds_vals[val])

    def c_modified_slot(self,val):
        self.slot(val)
        if self.style == "counter_slider":
          temp = [abs(x-val) for x in self.range.ds_vals]
          self.d_widget.set_slider(temp.index(min(temp)))

    class Dial(QtGui.QDial):
        """ Creates the range using a dial """
        def __init__(self, parent, ranges, slot):
            QtGui.QDial.__init__(self, parent)
            self.setRange(0, ranges.ds_steps-1)
            self.setSingleStep(ranges.step)
            self.setNotchTarget(1)
            self.setNotchesVisible(True)
            temp = [abs(x-ranges.default) for x in ranges.ds_vals]
            self.setValue(temp.index(min(temp)))
            self.valueChanged.connect(slot)

    class Slider(QtGui.QSlider):
        """ Creates the range using a slider """
        def __init__(self, parent, ranges, slot):
            QtGui.QSlider.__init__(self, QtCore.Qt.Horizontal, parent)
            self.setFocusPolicy(QtCore.Qt.NoFocus)
            self.setRange(0, ranges.ds_steps-1)
            temp = [abs(x-ranges.default) for x in ranges.ds_vals]
            self.setValue(temp.index(min(temp)))
            self.setPageStep(1)
            self.setSingleStep(1)
            self.setTickPosition(2)
            self.valueChanged.connect(slot)

        def mousePressEvent(self, event):
            if((event.button() == QtCore.Qt.LeftButton)):
                newVal = self.minimum() + ((self.maximum()-self.minimum()) * event.x()) / self.width()
                self.setValue(newVal)
                event.accept()
            QtGui.QSlider.mousePressEvent(self, event)

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
        def __init__(self, parent, ranges, s_slot, c_slot):
            QtGui.QWidget.__init__(self, parent)

            # Need another horizontal layout
            layout = Qt.QHBoxLayout()

            # Create a slider with the top-level widget as the parent
            self.slider = RangeWidget.Slider(parent,ranges,s_slot)
            layout.addWidget(self.slider)

            # Setup the counter
            self.counter = RangeWidget.Counter(parent,ranges,c_slot)
            layout.addWidget(self.counter)

            # Wire the events to each other
            #counter.valueChanged.connect(slider.setValue)
            #slider.valueChanged.connect(counter.setValue)
            self.counter.valueChanged.connect(c_slot)
            self.slider.valueChanged.connect(s_slot)

            self.setLayout(layout)

        def set_slider(self,val):
            self.slider.setValue(val)
        def set_counter(self,val):
            self.counter.setValue(val)







