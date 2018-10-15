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
""" Module to convert XML bindings to YAML bindings """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import click

from ..core import get_xml_candidates, ModToolUpdate
from ..tools import SequenceCompleter
from .base import block_name, run, cli_input, ModToolException


@click.command('update', short_help=ModToolUpdate.description)
@click.option('--complete', is_flag=True, default=None,
              help="Convert all the XML bindings to YAML.")
@block_name
def cli(**kwargs):
    """ Update the XML bindings to YAML bindings """
    kwargs['cli'] = True
    self = ModToolUpdate(**kwargs)
    click.secho("GNU Radio module name identified: " + self.info['modname'], fg='green')
    get_blockname(self)
    run(self)

def get_blockname(self):
    """ Returns the blockname for block to be updated """
    if self.info['complete']:
        return
    if self.info['blockname'] is None:
        block_candidates = get_xml_candidates()
        with SequenceCompleter(block_candidates):
            self.info['blockname'] = cli_input('Which block do you wish to update? : ')
    if not self.info['blockname'] or self.info['blockname'].isspace():
        raise ModToolException('Block name not specified!')
    if self.info['blockname'] not in block_candidates:
        choices = [x for x in block_candidates if self.info['blockname'] in x]
        if len(choices) > 0:
            click.secho("Suggested alternatives: "+str(choices), fg='yellow')
        raise ModToolException("The XML bindings does not exists!")
