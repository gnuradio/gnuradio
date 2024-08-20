#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to convert XML bindings to YAML bindings """


import click

from ..core import get_xml_candidates, ModToolUpdate
from ..tools import SequenceCompleter
from .base import block_name, run, cli_input, ModToolException


def xml_blockname_complete(ctx, param, incomplete: str):
    return sorted(
        name for name in get_xml_candidates()
        if name.startswith(incomplete)
    )


@click.command('update', short_help=ModToolUpdate.description)
@click.option('--complete', is_flag=True, default=None,
              help="Convert all the XML bindings to YAML.")
@click.option('-I', '--include-blacklisted', is_flag=True, default=None,
              help="Include XML files with blacklisted names in the conversion process")
@click.argument("blockname", nargs=1, required=False, metavar="BLOCK_NAME", shell_complete=xml_blockname_complete)
def cli(**kwargs):
    """ Update the XML bindings to YAML bindings """
    kwargs['cli'] = True
    self = ModToolUpdate(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    get_blockname(self)
    run(self)


def get_blockname(self):
    """ Returns the blockname for block to be updated """
    if self.info['complete']:
        return
    block_candidates = get_xml_candidates()
    if self.info['blockname'] is None:
        with SequenceCompleter(block_candidates):
            self.info['blockname'] = cli_input(
                'Which block do you wish to update? : ')
    if not self.info['blockname'] or self.info['blockname'].isspace():
        raise ModToolException('Block name not specified!')
    if self.info['blockname'] not in block_candidates:
        choices = [x for x in block_candidates if self.info['blockname'] in x]
        if len(choices) > 0:
            click.secho("Suggested alternatives: " + str(choices), fg='yellow')
        raise ModToolException("The XML bindings does not exists!")
