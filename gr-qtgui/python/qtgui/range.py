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
    def __init__(self, minv, maxv, step, default, min_length):
        self.min = float(minv)
        self.max = float(maxv)
        self.step = float(step)
        self.default = float(default)
        self.min_length = min_length
        self.find_precision()
        self.find_nsteps()

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

    def demap_range(self,val):
        if val > self.max:
          val = self.max
        if val < self.min:
          val = self.min
        return ((val-self.min)/self.step)

    def map_range(self,val):
        if val > self.nsteps:
          val = self.max
        if val < 0:
          val = 0
        return (val*self.step+self.min)

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
        else:
            self.d_widget = self.CounterSlider(self, self.range, self.ds_modified_slot, self.c_modified_slot)

        layout.addWidget(self.d_widget)
        self.setLayout(layout)

    def ds_modified_slot(self,val):
        nval = self.range.map_range(val)
        self.slot(nval)
        if self.style == "counter_slider":
          self.d_widget.set_counter(nval)

    def c_modified_slot(self,val):
        self.slot(val)
        if self.style == "counter_slider":
          temp = self.range.demap_range(val)
          if temp-int(temp) >= 0.5:
            self.d_widget.set_slider(int(temp)+1)
          else:
            self.d_widget.set_slider(int(temp))

    class Dial(QtGui.QDial):
        """ Creates the range using a dial """
        def __init__(self, parent, ranges, slot):
            QtGui.QDial.__init__(self, parent)
            self.setRange(0, ranges.nsteps-1)
            self.setSingleStep(1)
            self.setNotchesVisible(True)
            temp = ranges.demap_range(ranges.default)
            if temp-int(temp) >= 0.5:
              temp = int(temp)+1
            else:
              temp = int(temp)
            self.setValue(temp)
            self.valueChanged.connect(slot)

    class Slider(QtGui.QSlider):
        """ Creates the range using a slider """
        def __init__(self, parent, ranges, slot):
            QtGui.QSlider.__init__(self, QtCore.Qt.Horizontal, parent)
            self.setFocusPolicy(QtCore.Qt.NoFocus)
            self.setRange(0, ranges.nsteps-1)
            temp = ranges.demap_range(ranges.default)
            if temp-int(temp) >= 0.5:
              temp = int(temp)+1
            else:
              temp = int(temp)
            self.setValue(temp)
            self.setPageStep(1)
            self.setSingleStep(1)
            self.setTickPosition(2)
            if ranges.nsteps > ranges.min_length:
              self.setTickInterval(int(ranges.nsteps/ranges.min_length))
            else:
              self.setTickInterval(1)
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
            self.setKeyboardTracking(False)
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







