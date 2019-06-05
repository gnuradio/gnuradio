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

    def __init__(self, module_name=None, file_name=None, target_dir=None,
                 target_file=None, yaml_confirm=False, json_confirm=False,
                 **kwargs):
        """ __init__ """
        self.info = {}
        self.info['modname'] = module_name
        self.info['filename'] = file_name
        self.info['target_dir'] = target_dir
        self.info['target_file'] = target_file
        self.info['json_confirm'] = json_confirm
        self.info['yaml_confirm'] = yaml_confirm
        setup_cli_logger(logger)

    def _validate(self):
        """ Validates the arguments """
        if self.info['modname'] is None:
            raise BlockToolException("GNU Radio module name not specified")
        if self.info['filename'] is None:
            raise BlockToolException("File name not specified")
        if self.info['target_dir'] is None:
            raise BlockToolException("GNU Radio module not specified")
        if self.info['target_file'] is None:
            raise BlockToolException("File name not specified")

    def run(self):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')
