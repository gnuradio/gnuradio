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
""" Automatically create XML bindings for GRC from block code """

import click

from ..core import ModToolMakeXML
from ..core import get_block_candidates
from ..tools import SequenceCompleter
from .base import common_params, block_name, run


@click.command('makexml', short_help=ModToolMakeXML.description)
@common_params
@block_name
def cli(**kwargs):
    """
    \b
    Make an XML file for GRC block bindings

    Note: This does not work on python blocks
    """
    kwargs['cli'] = True
    self = ModToolMakeXML(**kwargs)
    click.secho("GNU Radio module name identified: " + self.info['modname'], fg='green')
    get_pattern(self)
    run(self)

def get_pattern(self):
    """ Get the regex pattern for block(s) to be parsed """
    if self.info['pattern'] is None:
        block_candidates = get_block_candidates()
        with SequenceCompleter(block_candidates):
            self.info['pattern'] = input(click.style('Which blocks do you want to parse? (Regex): ',
                                                     fg='blue'))
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
