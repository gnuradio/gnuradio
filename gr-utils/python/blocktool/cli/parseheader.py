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

import click

from blocktool.cli.base import run
from blocktool.core.parseheader import BlockHeaderParser


@click.command('parseheader', short_help=BlockHeaderParser.description)
@click.argument('file-path', nargs=1)
@click.option('-j', '--json-confirm', is_flag=True,
              help='Get a JSON output for the header')
@click.option('-y', '--yaml-confirm', is_flag=True,
              help='Get a YAML output for the header')
def cli(**kwargs):
    """
    \b
    Get parsed output for a header file from GNU Radio module
    """
    self = BlockHeaderParser(**kwargs)
    self.info['cli'] = True
    click.secho('Header file: ' + self.info['filename'], fg='green')
    get_json_file(self)
    get_yaml_file(self)
    run(self)


def get_json_file(self):
    """
    Get confirmation for the JSON file output
    """
    header_file = self.info['filename'].split('.')[0]
    block_module = self.info['modname'].split('-')[-1]
    if not self.info['json_confirm']:
        if click.confirm(click.style(
                'Do you require a JSON file?', fg='cyan')):
            self.info['json_confirm'] = True
    if self.info['json_confirm']:
        click.secho('Generating '+block_module+'_'+header_file +
                    '.json ...', fg='green')


def get_yaml_file(self):
    """
    Get confirmation for the YAML file output
    """
    header_file = self.info['filename'].split('.')[0]
    block_module = self.info['modname'].split('-')[-1]
    if not self.info['yaml_confirm']:
        if click.confirm(click.style(
                'Do you require a YAML file?', fg='cyan')):
            self.info['yaml_confirm'] = True
    if self.info['yaml_confirm']:
        click.secho("Generating "+block_module+"_"+header_file +
                    ".block.yml ...", fg='green')
