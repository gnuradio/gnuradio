#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Create Python bindings for GR block """


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
@click.option('--addl_includes', default=None,
              help='Comma separated list of additional include directories (default None)')
@click.option('-D', '--define_symbols', multiple=True, default=None,
              help='Set preprocessor defines')
@click.option('-u', '--update-hash-only', is_flag=True,
              help='If given, only the hash in the binding will be updated')
@common_params
@block_name(skip_grc=True, skip_lib=True, skip_python=True)
def cli(**kwargs):
    """
    \b
    Create Python bindings for GR C++ block
    """
    kwargs['cli'] = True
    self = ModToolGenBindings(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    get_pattern(self)
    run(self)


def get_pattern(self):
    """ Get the regex pattern for block(s) to be parsed """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates(self.info['modname'], skip_grc=True, skip_python=True)
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = cli_input(
                'Which blocks do you want to parse? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
