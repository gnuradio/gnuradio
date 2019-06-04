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

import os
import re
import glob
import logging
import itertools

from gnuradio import gr
from blocktool.cli.base import setup_cli_logger

logger = logging.getLogger('gnuradio.blocktool')


class BlockToolException(Exception):
    """ Standard exception for blocktool classes. """
    pass


class BlockTool(object):
    """ Base class for all blocktool command classes. """
    name = 'base'
    description = None

    def __init__(self, modname=None, filename=None, **kwargs):
        """ __init__ """
        self.info = {}
        self.info['modname'] = modname
        self.info['filename'] = filename
        setup_cli_logger(logger)

    def _validate(self):
        """ Validates the arguments """
        if self.info['modname'] is None:
            raise BlockToolException("Module name not specified")
        if self.info['filename'] is None:
            raise BlockToolException("File name not specified")

    def run(self):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')
