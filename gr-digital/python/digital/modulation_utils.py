#
# Copyright 2010 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#

"""
Miscellaneous utilities for managing mods and demods, as well as other items
useful in dealing with generalized handling of different modulations and demods.
"""
from __future__ import unicode_literals

import inspect


# Type 1 modulators accept a stream of bytes on their input and produce complex baseband output
_type_1_modulators = {}

def type_1_mods():
    return _type_1_modulators

def add_type_1_mod(name, mod_class):
    _type_1_modulators[name] = mod_class


# Type 1 demodulators accept complex baseband input and produce a stream of bits, packed
# 1 bit / byte as their output.  Their output is completely unambiguous.  There is no need
# to resolve phase or polarity ambiguities.
_type_1_demodulators = {}

def type_1_demods():
    return _type_1_demodulators

def add_type_1_demod(name, demod_class):
    _type_1_demodulators[name] = demod_class

# Also record the constellation making functions of the modulations
_type_1_constellations = {}

def type_1_constellations():
    return _type_1_constellations

def add_type_1_constellation(name, constellation):
    _type_1_constellations[name] = constellation


def extract_kwargs_from_options(function, excluded_args, options):
    """
    Given a function, a list of excluded arguments and the result of
    parsing command line options, create a dictionary of key word
    arguments suitable for passing to the function.  The dictionary
    will be populated with key/value pairs where the keys are those
    that are common to the function's argument list (minus the
    excluded_args) and the attributes in options.  The values are the
    corresponding values from options unless that value is None.
    In that case, the corresponding dictionary entry is not populated.

    (This allows different modulations that have the same parameter
    names, but different default values to coexist.  The downside is
    that --help in the option parser will list the default as None,
    but in that case the default provided in the __init__ argument
    list will be used since there is no kwargs entry.)

    Args:
        function: the function whose parameter list will be examined
        excluded_args: function arguments that are NOT to be added to the dictionary (sequence of strings)
        options: result of command argument parsing (optparse.Values)
    """
    
    # Try this in C++ ;)
    args, varargs, varkw, defaults = inspect.getargspec(function)
    d = {}
    for kw in [a for a in args if a not in excluded_args]:
        if hasattr(options, kw):
            if getattr(options, kw) is not None:
                d[kw] = getattr(options, kw)
    return d

def extract_kwargs_from_options_for_class(cls, options):
    """
    Given command line options, create dictionary suitable for passing to __init__
    """
    d = extract_kwargs_from_options(
        cls.__init__, ('self',), options)
    for base in cls.__bases__:
        if hasattr(base, 'extract_kwargs_from_options'):
            d.update(base.extract_kwargs_from_options(options))
    return d
