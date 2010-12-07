#
# Copyright 2010 Free Software Foundation, Inc.
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

from PyQt4 import QtGui
from PyQt4.QtCore import Qt

import converters

########################################################################
# Base class for all forms
########################################################################
class _form_base(QtGui.QWidget):
    def __init__(self, parent=None, converter=None, callback=None, value=None):
        QtGui.QWidget.__init__(self, parent)
        self._layout = QtGui.QBoxLayout(QtGui.QBoxLayout.LeftToRight, self)
        self._converter = converter
        self._callback = callback
        self._value = value

    def _add_widget(self, widget, label):
        if label:
            label_widget = QtGui.QLabel(self)
            label_widget.setText('%s: '%label)
            self._layout.addWidget(label_widget, True)
        self._layout.addWidget(widget, stretch=1)
        #disable callback, update, re-enable
        callback = self._callback
        self._callback = None
        self.set_value(self.get_value())
        self._callback = callback

    def get_value(self):
        return self._value

    def set_value(self, value):
        self._value = value
        self._base_update(self._converter.external_to_internal(value))

    def _base_update(self, int_val):
        self._update(int_val)

    def _base_handle(self, int_val):
        self._value = self._converter.internal_to_external(int_val)
        if self._callback: self._callback(self._value)

########################################################################
# Slider base class, shared by log and linear sliders
########################################################################
class _slider_base(_form_base):
    def __init__(self, label='', length=-1, num_steps=100, orient=Qt.Horizontal, **kwargs):
        _form_base.__init__(self, **kwargs)
        self._slider = QtGui.QSlider(parent=self)
        self._slider.setOrientation(orient)
        self._slider.setRange(0, num_steps)
        if length > 0:
            if orient == Qt.Horizontal:
                slider_size = self._slider.setWidth(length)
            if orient == Qt.Vertical:
                slider_size = self._slider.setHeight(length)
        self._add_widget(self._slider, label)
        self._slider.valueChanged.connect(self._handle)

    def _handle(self, event):
        value = self._slider.value()
        if self._cache != value: self._base_handle(value)
    def _update(self, value):
        self._cache = int(round(value))
        self._slider.setValue(self._cache)

########################################################################
# Linear slider form
########################################################################
class slider(_slider_base):
    """
    A generic linear slider.
    @param parent the parent widget
    @param value the default value
    @param label title label for this widget (optional)
    @param length the length of the slider in px (optional)
    @param orient Qt.Horizontal Veritcal (default=horizontal)
    @param start the start value
    @param stop the stop value
    @param num_steps the number of slider steps (or specify step_size)
    @param step_size the step between slider jumps (or specify num_steps)
    @param cast a cast function, int, or float (default=float)
    """
    def __init__(self, start, stop, step=None, num_steps=100, cast=float, **kwargs):
        assert step or num_steps
        if step is not None: num_steps = (stop - start)/step
        converter = converters.slider_converter(start=start, stop=stop, num_steps=num_steps, cast=cast)
        _slider_base.__init__(self, converter=converter, num_steps=num_steps, **kwargs)

########################################################################
# Text Box Form
########################################################################
class text_box(_form_base):
    """
    A text box form.
    @param parent the parent widget
    @param sizer add this widget to sizer if provided (optional)
    @param value the default value
    @param label title label for this widget (optional)
    @param converter forms.str_converter(), int_converter(), float_converter()...
    """
    def __init__(self, label='', converter=converters.eval_converter(), **kwargs):
        _form_base.__init__(self, converter=converter, **kwargs)
        self._text_box = QtGui.QLineEdit(self)
        self._default_style_sheet = self._text_box.styleSheet()
        self._text_box.textChanged.connect(self._set_color_changed)
        self._text_box.returnPressed.connect(self._handle)
        self._add_widget(self._text_box, label)

    def _set_color_default(self):
        self._text_box.setStyleSheet(self._default_style_sheet)

    def _set_color_changed(self, *args):
        self._text_box.setStyleSheet("QWidget { background-color: #EEDDDD }")

    def _handle(self): self._base_handle(str(self._text_box.text()))
    def _update(self, value):
        self._text_box.setText(value)
        self._set_color_default()
