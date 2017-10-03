# Copyright 2008-2017 Free Software Foundation, Inc.
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


class TemplateArg(str):
    """
    A cheetah template argument created from a param.
    The str of this class evaluates to the param's to code method.
    The use of this class as a dictionary (enum only) will reveal the enum opts.
    The __call__ or () method can return the param evaluated to a raw python data type.
    """

    def __new__(cls, param):
        value = param.to_code()
        instance = str.__new__(cls, value)
        setattr(instance, '_param', param)
        return instance

    def __getitem__(self, item):
        return str(self._param.get_opt(item)) if self._param.is_enum() else NotImplemented

    def __getattr__(self, item):
        if not self._param.is_enum():
            raise AttributeError()
        try:
            return str(self._param.get_opt(item))
        except KeyError:
            raise AttributeError()

    def __str__(self):
        return str(self._param.to_code())

    def __call__(self):
        return self._param.get_evaluated()
