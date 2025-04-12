# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
"""
Class method decorators.
"""

import functools

# pylint: disable=too-few-public-methods,invalid-name


class lazy_property:
    """
    Class method decorator, similar to @property. This causes a method that is
    declared as @lazy_property to be evaluated once, the first time it is called.
    Subsequent calls to this property will always return the cached value.

    Careful! Not suitable for properties that change at runtime.

    Example:

    >>> class Foo:
    ...     def __init__(self):
    ...         self.x = 5
    ...
    ...     @lazy_property
    ...     def xyz(self):
    ...         return complicated_slow_function(self.x)
    ...
    ...
    >>> f = Foo()
    >>> print(f.xyz) # Will give the result, but takes long to compute
    >>> print(f.xyz) # Blazing fast!
    >>> f.x = 7 # Careful! f.xyz will not be updated any more!
    >>> print(f.xyz) # Blazing fast, but returns the same value as before.
    """

    def __init__(self, func):
        self.func = func
        functools.update_wrapper(self, func)

    def __get__(self, instance, owner):
        if instance is None:
            return self
        value = self.func(instance)
        setattr(instance, self.func.__name__, value)
        return value
# pylint: enable=too-few-public-methods,invalid-name


def nop_write(prop):
    """
    Make this a property with a nop setter

    Effectively, makes a property read-only, with no error on write.
    """

    def nop(self, value):
        pass
    return prop.setter(nop)
