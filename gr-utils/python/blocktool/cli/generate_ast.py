#
# Copyright 2019 Free Software Foundation, Inc.
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
""" Module to add new blocks """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import re
import click

from blocktool.core.generate_ast import BlockToolGenerateAst
from blocktool.cli.base import run, cli_input, BlockToolException

@click.command('generate_ast')
@click.option('-m', '--module_type', type=click.Choice(BlockToolGenerateAst.module_types),
              help="One of {}.".format(', '.join(BlockToolGenerateAst.module_types)))
@click.option('-f', '--file-type',
              help=BlockToolGenerateAst.description, default="")
def cli(**kwargs):
    """Select a GNU Radio Module."""
    self = BlockToolGenerateAst(**kwargs)
    get_module_name(self)
    get_header(self)
    run(self)


def get_module_name(self):
    """ Get the regex of the GNU Radio module """
    if self.info['modname'] is None:
        click.secho(str(self.module_types), fg='yellow')
        while self.info['modname'] not in self.module_types:
            self.info['modname'] = cli_input("Enter GNU Radio module name: ")
            if 'gr' not in self.info['modname']:
                self.info['modname'] = 'gr-'+self.info['modname']
            if self.info['modname'] not in self.module_types:
                click.secho('Must be one of ' + str(self.module_types), fg='yellow')


def get_header(self):
    """ Get the regex of the header file """
    if self.info['filename'] is None:
        click.secho("GNU Radio module: " + self.info['modname'], fg='green')
        self.info['filename'] = cli_input("Enter public header file name from "+self.info['modname']+" module: ")
    if self.info['filename'] is "":
        raise BlockToolException('Enter a file name!')
    extension = self.info['filename'].split('.')[-1]
    if extension is not '.h' or '.cc':
        raise BlockToolException('Please enter a valid header file only!')
        

if __name__ == '__main__':
    cli()