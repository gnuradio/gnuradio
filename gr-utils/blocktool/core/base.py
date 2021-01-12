#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Base class for the modules """


from abc import ABC, abstractmethod


class BlockToolException(Exception):
    """ Standard exception for blocktool classes. """
    pass


class BlockTool(ABC):
    """ Base class for all blocktool command classes. """
    name = 'base'
    description = None

    def __init__(self, modname=None, filename=None, targetdir=None,
                 target_file=None, module=None, impldir=None, impl_file=None,
                 yaml=False, json=False, include_paths=None, **kwargs):
        """ __init__ """
        pass

    def _validate(self):
        """ Validates the arguments """
        pass

    @abstractmethod
    def run_blocktool(self):
        """ Override this. """
        pass
