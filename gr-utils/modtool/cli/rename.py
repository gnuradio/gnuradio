#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to rename blocks """


import re

import click

from ..core import get_block_candidates, ModToolRename, validate_name
from ..tools import SequenceCompleter
from .base import common_params, block_name, run, cli_input, ModToolException


@click.command('rename', short_help=ModToolRename.description)
@common_params
@block_name()
@click.argument('new-name', metavar="NEW-BLOCK-NAME", nargs=1, required=False)
def cli(**kwargs):
    """
    \b
    Rename a block inside a module.

    The argument NEW-BLOCK-NAME is the new name of the block.
    """
    kwargs['cli'] = True
    self = ModToolRename(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    # first make sure the old block name is provided
    get_oldname(self)
    click.secho("Block/code to rename identifier: " +
                self.info['oldname'], fg='green')
    self.info['fulloldname'] = self.info['modname'] + \
        '_' + self.info['oldname']
    # now get the new block name
    get_newname(self)
    click.secho("Block/code identifier: " + self.info['newname'], fg='green')
    self.info['fullnewname'] = self.info['modname'] + \
        '_' + self.info['newname']
    run(self)


def get_oldname(self):
    """ Get the old block name to be replaced """
    block_candidates = get_block_candidates(self.info['modname'])
    if self.info['oldname'] is None:
        with SequenceCompleter(block_candidates):
            while not self.info['oldname'] or self.info['oldname'].isspace():
                self.info['oldname'] = cli_input("Enter name of block/code to rename " +
                                                 "(without module name prefix): ")
    if self.info['oldname'] not in block_candidates:
        choices = [x for x in block_candidates if self.info['oldname'] in x]
        if len(choices) > 0:
            click.secho("Suggested alternatives: " + str(choices), fg='yellow')
        raise ModToolException("Blockname for renaming does not exists!")
    validate_name('block', self.info['oldname'])


def get_newname(self):
    """ Get the new block name """
    if self.info['newname'] is None:
        while not self.info['newname'] or self.info['newname'].isspace():
            self.info['newname'] = cli_input("Enter name of block/code " +
                                             "(without module name prefix): ")
    validate_name('block', self.info['newname'])
