# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import functools


class lazy_property(object):

    def __init__(self, func):
        self.func = func
        functools.update_wrapper(self, func)

    def __get__(self, instance, owner):
        if instance is None:
            return self
        value = self.func(instance)
        setattr(instance, self.func.__name__, value)
        return value


def nop_write(prop):
    """Make this a property with a nop setter"""

    def nop(self, value):
        pass
    return prop.setter(nop)
