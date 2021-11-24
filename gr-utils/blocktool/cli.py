#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to generate parsed header output data """


import os
import sys
import json
import logging

import click
from click import ClickException

from gnuradio.modtool.core import yaml_generator

from .core.parseheader import BlockHeaderParser

LOGGER = logging.getLogger(__name__)


class BlockToolException(ClickException):
    """ Exception class for enhanced CLI interface """

    def show(self, file=None):
        """ displays the colored message """
        click.secho('BlockToolException: {}'.format(
            self.format_message()), fg='red')


def run_blocktool(module):
    """Call the run function of the core modules."""
    try:
        module.run_blocktool()
    except BlockToolException as err:
        click.echo(err, file=sys.stderr)
        exit(1)


@click.command('parseheader',
               short_help='Generate the parsed output for the header file or directory in a specified format')
@click.argument('file-path', nargs=1)
@click.option('--yaml', is_flag=True,
              help='If given, a YAML response will be printed, else default json will be printed')
@click.option('-c', '--blocktool-comments', is_flag=True,
              help='blocktool helper comments will be added in the header file')
@click.option('-o', '--output', is_flag=True,
              help='If given, a file with desired output format will be generated')
@click.option('-I', '--include_paths', default=None,
              help='Comma separated list of include paths for header files')
def cli(**kwargs):
    """
    Block header parsing tool.
    \b
    A tool that can be used to automatically parse the headers in GNU Radio project
    or the OOT modules
    """
    kwargs['modtool'] = False
    if os.path.isfile(kwargs['file_path']):
        parser = BlockHeaderParser(**kwargs)
        run_blocktool(parser)
        if kwargs['yaml']:
            parser.yaml = True
            yaml_generator(parser, **kwargs)
        else:
            parser.json_confirm = True
            json_generator(parser, **kwargs)
    elif os.path.isdir(kwargs['file_path']):
        parse_directory(**kwargs)
    else:
        raise BlockToolException('Invalid file or directory path.')


def json_generator(parser, **kwargs):
    """
    Generate JSON file for the block header
    """
    header = parser.filename.split('.')[0]
    block = parser.modname.split('-')[-1]
    if kwargs['output']:
        json_file = os.path.join('.', block + '_' + header + '.json')
        with open(json_file, 'w') as _file:
            json.dump(parser.parsed_data, _file, indent=4)
    else:
        print(json.dumps(parser.parsed_data, indent=4))


def parse_directory(**kwargs):
    """
    Get parsed json and yaml output for complete header directory
    """
    kwargs['output'] = True
    dir_path = kwargs['file_path']
    dir_path = os.path.abspath(dir_path)
    list_header = []
    dir_name = os.path.basename(dir_path)
    for _header in os.listdir(dir_path):
        if _header.endswith('.h') and os.path.isfile(os.path.join(dir_path, _header)):
            list_header.append(os.path.join(dir_path, _header))
    list_header = sorted(list_header)
    if list_header:
        for header_path in list_header:
            kwargs['file_path'] = header_path
            header = os.path.basename(header_path)
            try:
                parse_dir = BlockHeaderParser(**kwargs)
                parse_dir.yaml = True
                parse_dir.json = True
                run_blocktool(parse_dir)
                yaml_generator(parse_dir, **kwargs)
                if not kwargs['modtool']:
                    json_generator(parse_dir, **kwargs)
            except:
                logging.basicConfig(level=logging.DEBUG,
                                    filename=os.path.join('.', dir_name + '_log.out'))
                logging.exception(
                    'Log for Exception raised for the header: {}\n'.format(header))
                click.secho('Parsing unsuccessful: {}'.format(
                    header), fg='yellow')
    else:
        raise BlockToolException(
            'Invalid directory! No header found to be parsed')
