#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Remove blocks module """


import click

from ..core import get_block_candidates, ModToolRemove
from ..tools import SequenceCompleter
from .base import common_params, block_name, run, cli_input


@click.command('remove', short_help=ModToolRemove.description)
@common_params
@block_name()
def cli(**kwargs):
    """ Remove block (delete files and remove Makefile entries) """
    kwargs['cli'] = True
    self = ModToolRemove(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    get_pattern(self)
    run(self)


def get_pattern(self):
    """ Returns the regex pattern for block(s) to be removed """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates(self.info['modname'])
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = cli_input(
                'Which blocks do you want to delete? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
