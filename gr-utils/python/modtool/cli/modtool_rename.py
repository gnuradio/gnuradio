#
# Copyright 2018 Free Software Foundation, Inc.
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
""" Module to rename blocks """

import re
from types import SimpleNamespace

import click

from gnuradio.modtool.core.modtool_base import ModTool, ModToolException
from gnuradio.modtool.core.modtool_rename import ModToolRename
from .modtool_base import common_params, block_name, run


@click.command('rename', short_help=ModToolRename.description)
@common_params
@block_name
@click.argument('new-name', metavar="NEW-BLOCK-NAME", nargs=1, required=False)
def cli(**kwargs):
    """
    \b
    Rename a block inside a module.

    The argument NEW-BLOCK-NAME is the new name of the block.
    """
    options = SimpleNamespace(**kwargs)
    self = ModToolRename()
    self._cli = True

    ModTool.setup(self, options)

    if ((self._skip_subdirs['lib'] and self._info['lang'] == 'cpp')
            or (self._skip_subdirs['python'] and self._info['lang'] == 'python')):
        raise ModToolException('Missing or skipping relevant subdir.')

    # first make sure the old block name is provided
    self._info['oldname'] = options.blockname
    if self._info['oldname'] is None:
        self._info['oldname'] = input("Enter name of block/code to rename (without module name prefix): ")
    if not re.match('[a-zA-Z0-9_]+', self._info['oldname']):
        raise ModToolException('Invalid block name.')
    print("Block/code to rename identifier: " + self._info['oldname'])
    self._info['fulloldname'] = self._info['modname'] + '_' + self._info['oldname']

    # now get the new block name
    if options.new_name is None:
        self._info['newname'] = input("Enter name of block/code (without module name prefix): ")
    else:
        self._info['newname'] = options.new_name
    if not re.match('[a-zA-Z0-9_]+', self._info['newname']):
        raise ModToolException('Invalid block name.')
    print("Block/code identifier: " + self._info['newname'])
    self._info['fullnewname'] = self._info['modname'] + '_' + self._info['newname']

    run(self, options)
