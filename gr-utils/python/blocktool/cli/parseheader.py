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
""" CLI module for the tool """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import click

from modtool.tools.util_functions import SequenceCompleter

from blocktool.cli.base import run, cli_input, BlockToolException
from blocktool.core.parseheader import BlockHeaderParser


@click.command('parseheader', short_help=BlockHeaderParser.description)
@click.option('-m', '--module-name',
              type=click.Choice(BlockHeaderParser.module_types),
                    help="One of {}.".format(
                    ', '.join(BlockHeaderParser.module_types)))
@click.option('-j', '--json-confirm', is_flag=True,
              help="Get a JSON output for the header")
@click.option('-y', '--yaml-confirm', is_flag=True,
              help="Get a YAML output for the header")
def cli(**kwargs):
    """
    \b
    Get parsed output for a header file from GNU Radio module
    """
    kwargs['cli_confirm'] = True
    self = BlockHeaderParser(**kwargs)
    get_module_name(self)
    get_header_file(self)
    get_json_file(self)
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
                self.info['modname'] = cli_input(
                    "Enter GNU Radio module name: ")
                if self.info['modname'] not in self.module_types:
                    click.secho('Must be one of ' +
                                str(self.module_types), fg='yellow')


def get_header_file(self):
    """
    Get the public header file to be parsed
    """
    if 'gr' not in self.info['modname']:
        self.info['modname'] = 'gr-'+self.info['modname']
    self.info['target_dir'] = os.path.join(self.target_dir,
                                           self.info['modname'], 'include',
                                           'gnuradio',
                                           self.info['modname'].split('-')[-1])
    for header in os.listdir(self.info['target_dir']):
        if header.endswith('.h'):
            self.header_list.append(header)

    if self.info['filename'] is None:
        click.secho("GNU Radio module: " + self.info['modname'], fg='green')
        with SequenceCompleter(self.header_list):
            self.info['filename'] = cli_input(
                                        "Enter public header file name from " +
                                        self.info['modname']+" module: ")
    if self.info['filename'] is "":
        raise BlockToolException('Enter a file name!')
    if not self.info['filename'].endswith('.h'):
        raise BlockToolException('header files have extension .h only!')
    if self.info['filename'] not in self.header_list:
        raise BlockToolException(
            'Please enter a header file from '+self.info['modname']+' only!')
    else:
        self.info['target_file'] = os.path.join(self.info['target_dir'],
                                                self.info['filename'])
        click.secho("Header file: " + self.info['filename'], fg='green')


def get_json_file(self):
    """
    Get confirmation for the JSON file output
    """
    header_file = self.info['filename'].split('.')[0]
    block_module = self.info['modname'].split('-')[-1]
    if self.info['json_confirm'] is False:
        if click.confirm(click.style(
                'Do you require a JSON file?', fg='cyan')):
            self.info['json_confirm'] = True
            click.secho("Generating "+block_module+"_"+header_file +
                        ".json ...", fg='green')
    else:
        click.secho("Generating "+block_module+"_"+header_file +
                    ".json ...", fg='green')


def get_yaml_file(self):
    """
    Get confirmation for the YAML file output
    """
    header_file = self.info['filename'].split('.')[0]
    block_module = self.info['modname'].split('-')[-1]
    if self.info['yaml_confirm'] is False:
        if click.confirm(click.style(
                'Do you require a YAML file?', fg='cyan')):
            self.info['yaml_confirm'] = True
            click.secho("Generating "+block_module+"_"+header_file +
                        ".block.yml ...", fg='green')
    else:
        click.secho("Generating "+block_module+"_"+header_file +
                    ".block.yml ...", fg='green')
