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
""" Remove blocks module """

import click

from ..core import ModToolRemove
from .base import common_params, block_name, run


@click.command('remove', short_help=ModToolRemove.description)
@common_params
@block_name
def cli(**kwargs):
    """ Remove block (delete files and remove Makefile entries) """
    kwargs['cli'] = True
    self = ModToolRemove(**kwargs)
    get_pattern(self)
    run(self)

def get_pattern(self):
    if self.info['pattern'] is None:
        self.info['pattern'] = input('Which blocks do you want to delete? (Regex): ')
    if not self.info['pattern'] or self.info['pattern'].isspace():
        self.info['pattern'] = '.'
