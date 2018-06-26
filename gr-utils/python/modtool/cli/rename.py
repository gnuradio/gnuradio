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

import click

from ..core import ModToolException, ModToolRename
from ..core import get_block_candidates
from ..tools import SequenceCompleter
from .base import common_params, block_name, run


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
    kwargs['cli'] = True
    self = ModToolRename(**kwargs)
    # first make sure the old block name is provided
    get_oldname(self)
    click.echo("Block/code to rename identifier: " + self.info['oldname'])
    self.info['fulloldname'] = self.info['modname'] + '_' + self.info['oldname']
    # now get the new block name
    get_newname(self)
    click.echo("Block/code identifier: " + self.info['newname'])
    self.info['fullnewname'] = self.info['modname'] + '_' + self.info['newname']
    run(self)

def get_oldname(self):
    block_candidates = get_block_candidates()
    if self.info['oldname'] is None:
        with SequenceCompleter(block_candidates):
            while not self.info['oldname'] or self.info['oldname'].isspace():
                self.info['oldname'] = input("Enter name of block/code to rename (without module name prefix): ")
    if self.info['oldname'] not in block_candidates:
        choices = [x for x in block_candidates if self.info['oldname'] in x]
        if len(choices)>0:
            print("Suggested alternatives: "+str(choices))
        raise ModToolException("Blockname for renaming does not exists!")
    if not re.match('[a-zA-Z0-9_]+', self.info['oldname']):
        raise ModToolException('Invalid block name.')

def get_newname(self):
    if self.info['newname'] is None:
        while not self.info['newname'] or self.info['newname'].isspace():
            self.info['newname'] = input("Enter name of block/code (without module name prefix): ")
    if not re.match('[a-zA-Z0-9_]+', self.info['newname']):
        raise ModToolException('Invalid block name.')
