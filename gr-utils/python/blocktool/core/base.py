#
# Copyright 2019 Free Software Foundation, Inc.
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
""" Base class for the modules """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

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
