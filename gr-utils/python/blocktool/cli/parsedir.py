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
import logging
import click

from blocktool.cli.base import run
from blocktool.core.parseheader import BlockHeaderParser

LOGGER = logging.getLogger(__name__)


@click.command('parsedir', short_help='Parses complete header directory')
@click.argument('dir-path', nargs=1)
def cli(**kwargs):
    """
    \b
    Get parsed output for every header file from the GNU Radio module
    """
    parse_directory(**kwargs)


def parse_directory(**kwargs):
    """
    Get parsed json and yaml output for complete header directory
    """
    dir_path = kwargs['dir_path']
    dir_path = os.path.abspath(dir_path)
    if not os.path.isdir(dir_path):
        raise OSError
    list_header = []
    dir_name = os.path.basename(dir_path)
    for _header in os.listdir(dir_path):
        if _header.endswith('.h') and os.path.isfile(os.path.join(dir_path, _header)):
            list_header.append(os.path.join(dir_path, _header))
    list_header = sorted(list_header)
    for header_path in list_header:
        kwargs['file_path'] = header_path
        header = os.path.basename(header_path)
        try:
            self = BlockHeaderParser(**kwargs)
            self.info['cli'] = True
            self.info['yaml_confirm'] = True
            self.info['json_confirm'] = True
            run(self)
            click.secho('Parsing successful: {}'.format(header), fg='green')
        except:
            logging.basicConfig(level=logging.DEBUG,
                                filename=os.path.join('.', dir_name+'_log.out'))
            logging.exception(
                'Log for Exception raised for the header: {}\n'.format(header))
            click.secho('Parsing unsuccessful: {}'.format(header), fg='yellow')
