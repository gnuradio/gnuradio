#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Automatically create YAML bindings for GRC from block code """


import os
import click

try:
    from gnuradio.blocktool import BlockHeaderParser
    from gnuradio.blocktool.core.base import BlockToolException
except ImportError:
    have_blocktool = False
else:
    have_blocktool = True

from ..core import get_block_candidates, ModToolMakeYAML, yaml_generator
from ..tools import SequenceCompleter
from .base import common_params, block_name, run, cli_input


@click.command('makeyaml', short_help=ModToolMakeYAML.description)
@click.option('-b', '--blocktool', is_flag=True,
              help='Use blocktool support to print yaml output. FILE PATH mandatory if used.')
@click.option('-o', '--output', is_flag=True,
              help='If given, a file with desired output format will be generated')
@common_params
@block_name(skip_grc=True, skip_include=True, skip_python=True)  # TODO: python blocks
def cli(**kwargs):
    """
    \b
    Make an YAML file for GRC block bindings

    Note: This does not work on python blocks
    """
    kwargs['cli'] = True
    if kwargs['blocktool']:
        kwargs['modtool'] = True
        if kwargs['blockname'] is None:
            raise BlockToolException(
                'Missing argument FILE PATH with blocktool flag')
        kwargs['file_path'] = os.path.abspath(kwargs['blockname'])
        if os.path.isfile(kwargs['file_path']):
            parse_yml = BlockHeaderParser(**kwargs)
            parse_yml.run_blocktool()
            parse_yml.cli = True
            parse_yml.yaml = True
            yaml_generator(parse_yml, **kwargs)
        else:
            raise BlockToolException('Invalid file path.')
    else:
        self = ModToolMakeYAML(**kwargs)
        click.secho("GNU Radio module name identified: " +
                    self.info['modname'], fg='green')
        get_pattern(self)
        run(self)


def get_pattern(self):
    """ Get the regex pattern for block(s) to be parsed """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates(self.info['modname'], skip_include=True,
                                                skip_python=True, skip_grc=True)
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = cli_input(
                'Which blocks do you want to parse? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
