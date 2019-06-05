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

import os
import click

from blocktool.core.parseheader import BlockToolGenerateAst
from blocktool.cli.base import run, cli_input, BlockToolException
from modtool.tools.util_functions import SequenceCompleter


@click.command('parseheader', short_help=BlockToolGenerateAst.description)
@click.option('-m', '--module-name', type=click.Choice(BlockToolGenerateAst.module_types),
              help="One of {}.".format(', '.join(BlockToolGenerateAst.module_types)))
def cli(**kwargs):
    """
    \b
    Get parsed output for a header file from GNU Radio module
    """
    self = BlockToolGenerateAst(**kwargs)
    get_module_name(self)
    get_header_file(self)
    get_yaml_file(self)
    run(self)


def get_module_name(self):
    """
    Get the regex of the GNU Radio module
    """
    if self.info['modname'] is None:
        click.secho(str(self.module_types), fg='yellow')
        with(SequenceCompleter(self.module_types)):
            while self.info['modname'] not in self.module_types:
                self.info['modname'] = cli_input("Enter GNU Radio module name: ")
                if self.info['modname'] not in self.module_types:
                    click.secho('Must be one of '+
                        str(self.module_types), fg='yellow')


def get_header_file(self):
    """
    Get the public header file to be parsed
    """
    if 'gr' not in self.info['modname']:
        self.info['modname'] = 'gr-'+self.info['modname']
    self.info['target_dir'] = os.path.join('.', '..', '..', self.info['modname'],
                    'include', 'gnuradio', self.info['modname'].split('-')[-1])
    header_list = os.listdir(self.info['target_dir'])
    if 'CMakeLists.txt' in header_list:
        header_list.remove('CMakeLists.txt')

    if self.info['filename'] is None:
        click.secho("GNU Radio module: " + self.info['modname'], fg='green')
        with SequenceCompleter(header_list):
            self.info['filename'] = cli_input("Enter public header file name from "+
                                            self.info['modname']+" module: ")
    if self.info['filename'] is "":
        raise BlockToolException('Enter a file name!')
    extension = self.info['filename'].split('.')[-1]
    if extension is not 'h':
        raise BlockToolException('header files have extension .h only!')
    if self.info['filename'] not in header_list:
        raise BlockToolException(
            'Please enter a header file from '+self.info['modname']+' only!')
    else:
        self.info['target_file'] = os.path.join(self.info['target_dir'],
                                        self.info['filename'])


def get_yaml_file(self):
    """
    Get confirmation for the YAML file output
    """
    if self.info['yaml_confirm'] is False:
        click.secho("Header file: " + self.info['filename'], fg='green')
        if click.confirm(click.style(
            'Do you require a YAML along with a JSON file ?', fg='cyan')):
            self.info['yaml_confirm'] = True


if __name__ == '__main__':
    cli()
