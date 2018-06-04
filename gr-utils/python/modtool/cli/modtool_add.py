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
""" Module to add new blocks """

import sys
import re

import click

from .modtool_base import common_params, block_name, run
from gnuradio.modtool.core.modtool_add import ModToolAdd
from gnuradio.modtool.core.modtool_base import ModToolException
from gnuradio.modtool.core.util_functions import SequenceCompleter


@click.command('add')
@click.option('-t', '--block-type', type=click.Choice(ModToolAdd()._block_types),
              help="One of {}.".format(', '.join(ModToolAdd()._block_types)))
@click.option('--license-file',
              help="File containing the license header for every source code file.")
@click.option('--copyright',
              help="Name of the copyright holder (you or your company) MUST be a quoted string.")
@click.option('--argument-list',
              help="The argument list for the constructor and make functions.")
@click.option('--add-python-qa', is_flag=True, default=None,
              help="If given, Python QA code is automatically added if possible.")
@click.option('--add-cpp-qa', is_flag=True, default=None,
              help="If given, C++ QA code is automatically added if possible.")
@click.option('--skip-cmakefiles', is_flag=True,
              help="If given, only source files are written, but CMakeLists.txt files are left unchanged.")
@click.option('-l', '--lang', type=click.Choice(ModToolAdd().language_candidates),
              help="Programming Language")
@common_params
@block_name
def cli(**kwargs):
    """Adds a block to the out-of-tree module."""
    setup(**kwargs)


def setup(**kwargs):
    if kwargs['block_type'] is None:
        print (str(ModToolAdd()._block_types))
        with SequenceCompleter(sorted(ModToolAdd()._block_types)):
            while kwargs['block_type'] not in ModToolAdd()._block_types:
                kwargs['block_type'] = click.prompt("Enter block type")
                if kwargs['block_type'] not in ModToolAdd()._block_types:
                    print ('Must be one of ' + str(ModToolAdd()._block_types))

    # Allow user to specify language interactively if not set
    if kwargs['lang'] is None:
        with SequenceCompleter(ModToolAdd().language_candidates):
            while kwargs['lang'] not in ModToolAdd().language_candidates:
                kwargs['lang'] = click.prompt("Language (python/cpp)")
    if kwargs['lang'] == 'c++':
        kwargs['lang'] = 'cpp'

    click.echo("Language: {}".format({'cpp': 'C++', 'python': 'Python'}[kwargs['lang']]))

    if kwargs['blockname'] is None:
        kwargs['blockname'] = click.prompt("Enter name of block/code (without module name prefix)")
    if not re.match('[a-zA-Z0-9_]+', kwargs['blockname']):
        raise ModToolException('Invalid block name.')
    click.echo("Block/code identifier: " + kwargs['blockname'])

    if kwargs['argument_list'] is not None:
        kwargs['arglist'] = kwargs['argument_list']
    else:
        kwargs['arglist'] = input('Enter valid argument list, including default arguments: ')

    run(ModToolAdd, **kwargs)
