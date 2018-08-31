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


class Evaluated(object):
    def __init__(self, expected_type, default, name=None):
        self.expected_type = expected_type
        self.default = default

        self.name = name or 'evaled_property_{}'.format(id(self))
        self.eval_function = self.default_eval_func

    @property
    def name_raw(self):
        return '_' + self.name

    def default_eval_func(self, instance):
        raw = getattr(instance, self.name_raw)
        try:
            value = instance.parent_block.evaluate(raw)
        except Exception as error:
            if raw:
                instance.add_error_message("Failed to eval '{}': {}".format(raw, error))
            return self.default

        if not isinstance(value, self.expected_type):
            instance.add_error_message("Can not cast evaluated value '{}' to type {}"
                                       "".format(value, self.expected_type))
            return self.default
        # print(instance, self.name, raw, value)
        return value

    def __call__(self, func):
        self.name = func.__name__
        self.eval_function = func
        return self

    def __get__(self, instance, owner):
        if instance is None:
            return self
        attribs = instance.__dict__
        try:
            value = attribs[self.name]
        except KeyError:
            value = attribs[self.name] = self.eval_function(instance)
        return value

    def __set__(self, instance, value):
        attribs = instance.__dict__
        value = value or self.default
        if isinstance(value, six.text_type) and value.startswith('${') and value.endswith('}'):
            attribs[self.name_raw] = value[2:-1].strip()
        else:
            attribs[self.name] = type(self.default)(value)

    def __delete__(self, instance):
        attribs = instance.__dict__
        if self.name_raw in attribs:
            attribs.pop(self.name, None)


class EvaluatedEnum(Evaluated):
    def __init__(self, allowed_values, default=None, name=None):
        if isinstance(allowed_values, six.string_types):
            allowed_values = set(allowed_values.split())
        self.allowed_values = allowed_values
        default = default if default is not None else next(iter(self.allowed_values))
        super(EvaluatedEnum, self).__init__(str, default, name)

    def default_eval_func(self, instance):
        value = super(EvaluatedEnum, self).default_eval_func(instance)
        if value not in self.allowed_values:
            instance.add_error_message("Value '{}' not in allowed values".format(value))
            return self.default
        return value


class EvaluatedPInt(Evaluated):
    def __init__(self, name=None):
        super(EvaluatedPInt, self).__init__(int, 1, name)

    def default_eval_func(self, instance):
        value = super(EvaluatedPInt, self).default_eval_func(instance)
        if value < 1:
            # todo: log
            return self.default
        return value


class EvaluatedFlag(Evaluated):
    def __init__(self, name=None):
        super(EvaluatedFlag, self).__init__((bool, int), False, name)


def setup_names(cls):
    for name, attrib in cls.__dict__.items():
        if isinstance(attrib, Evaluated):
            attrib.name = name
    return cls
