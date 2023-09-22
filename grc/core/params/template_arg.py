# Copyright 2008-2017 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


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
