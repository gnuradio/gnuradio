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
from PyQt4 import QtCore

import converters

class _form_base(QtGui.QWidget):
    def __init__(self, parent=None, converter=None, callback=None, value=None):
        QtGui.QWidget.__init__(self, parent)
        self._converter = converter
        self._callback = callback
        self.set_value(value)

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

class _slider_base(_form_base):
    def __init__(self, label='', length=-1, num_steps=100, orient=QtCore.Qt.Horizontal, **kwargs):
        _form_base.__init__(self, **kwargs)
        self._slider = QtGui.QSlider(parent=self)
        self._slider.setOrientation(orient)
        self._slider.setRange(0, num_steps)
        if length > 0:
            if orient == QtCore.Qt.Horizontal:
                slider_size = self._slider.setWidth(length)
            if orient == QtCore.Qt.Vertical:
                slider_size = self._slider.setHeight(length)
        self.slider.valueChanged.connect(self._handle)

    def _handle(self, event): self._base_handle(self._slider.value())
    def _update(self, value): self._slider.setValue(int(round(value)))

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
    def __init__(self, start, stop, num_steps=100, step_size=None, cast=float, **kwargs):
        assert step_size or num_steps
        if step_size is not None: num_steps = (stop - start)/step_size
        converter = converters.slider_converter(start=start, stop=stop, num_steps=num_steps, cast=cast)
        _slider_base.__init__(self, converter=converter, num_steps=num_steps, **kwargs)
