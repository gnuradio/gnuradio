#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Disable blocks module """


import click

from ..core import get_block_candidates, ModToolDisable
from ..tools import SequenceCompleter
from .base import common_params, block_name, run, cli_input


@click.command('disable', short_help=ModToolDisable.description)
@common_params
@block_name()
def cli(**kwargs):
    """Disable a block (comments out CMake entries for files)"""
    kwargs['cli'] = True
    self = ModToolDisable(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    get_pattern(self)
    run(self)


def get_pattern(self):
    """ Get the regex pattern for block(s) to be disabled """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates(self.info['modname'])
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = cli_input(
                'Which blocks do you want to disable? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
