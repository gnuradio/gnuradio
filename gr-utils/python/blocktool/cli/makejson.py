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
""" Module to generate JSON file from the parsed data """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import json
import click

from blocktool.cli.base import run
from blocktool.core.parseheader import BlockHeaderParser


@click.command('makejson', short_help='Generate a JSON file from a block header')
@click.argument('file-path', nargs=1)
def cli(**kwargs):
    """
    \b
    Get parsed output for a header file from GNU Radio module
    """
    self = BlockHeaderParser(**kwargs)
    self.info['cli'] = True
    self.info['json_confirm'] = True
    click.secho('Header file: {}'.format(self.info['filename']), fg='green')
    run(self)
    json_generator(self)


def json_generator(self):
    """
    Generate JSON file for the block header
    """
    header_file = self.info['filename'].split('.')[0]
    block_module = self.info['modname'].split('-')[-1]
    click.secho('Successfully generated {}_{}.json'.format(
        block_module, header_file), fg='green')
    json_file = os.path.join('.', block_module+'_'+header_file + '.json')
    with open(json_file, 'w') as _file:
        json.dump(self.parsed_data, _file, indent=4)
    _file.close()
