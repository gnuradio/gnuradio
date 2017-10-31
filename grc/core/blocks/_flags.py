# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

import six


class Flags(six.text_type):

    THROTTLE = 'throttle'
    DISABLE_BYPASS = 'disable_bypass'
    NEED_QT_GUI = 'need_qt_gui'
    DEPRECATED = 'deprecated'
    NOT_DSP = 'not_dsp'

    def __getattr__(self, item):
        return item in self

    def __add__(self, other):
        if not isinstance(other, six.string_types):
            return NotImplemented
        return self.__class__(str(self) + other)

    __iadd__ = __add__
