#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Create Python bindings for GR block """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import click

try:
    from gnuradio.blocktool import BlockHeaderParser
    from gnuradio.blocktool.core.base import BlockToolException
except ImportError:
    have_blocktool = False
else:
    have_blocktool = True

from ..core import ModToolGenBindings, get_block_candidates
from ..tools import SequenceCompleter
from .base import common_params, block_name, run, cli_input


@click.command('bind', short_help=ModToolGenBindings.description)
@click.option('-o', '--output', is_flag=True,
              help='If given, a file with desired output format will be generated')
@common_params
@block_name
def cli(**kwargs):
    """
    \b
    Create Python bindings for GR C++ block
    """
    kwargs['cli'] = True
    self = ModToolGenBindings(**kwargs)
    click.secho("GNU Radio module name identified: " + self.info['modname'], fg='green')
    get_pattern(self)
    run(self)

def get_pattern(self):
    """ Get the regex pattern for block(s) to be parsed """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates()
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = cli_input('Which blocks do you want to parse? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
