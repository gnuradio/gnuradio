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

import re

import click

from ..core import ModToolAdd
from ..core import ModToolException
from ..tools import SequenceCompleter, ask_yes_no
from .base import common_params, block_name, run


@click.command('add')
@click.option('-t', '--block-type', type=click.Choice(ModToolAdd._block_types),
              help="One of {}.".format(', '.join(ModToolAdd._block_types)))
@click.option('--license-file',
              help="File containing the license header for every source code file.")
@click.option('--copyright',
              help="Name of the copyright holder (you or your company) MUST be a quoted string.")
@click.option('--argument-list', default="",
              help="The argument list for the constructor and make functions.")
@click.option('--add-python-qa', is_flag=True, default=None,
              help="If given, Python QA code is automatically added if possible.")
@click.option('--add-cpp-qa', is_flag=True, default=None,
              help="If given, C++ QA code is automatically added if possible.")
@click.option('--skip-cmakefiles', is_flag=True,
              help="If given, only source files are written, but CMakeLists.txt files are left unchanged.")
@click.option('-l', '--lang', type=click.Choice(ModToolAdd.language_candidates),
              help="Programming Language")
@common_params
@block_name
def cli(**kwargs):
    """Adds a block to the out-of-tree module."""
    kwargs['cli'] = True
    self = ModToolAdd(kwargs)
    if self._info['blocktype'] is None:
        click.echo(str(self._block_types))
        with SequenceCompleter(self._block_types):
            while self._info['blocktype'] not in self._block_types:
                self._info['blocktype'] = input("Enter block type: ")
                if self._info['blocktype'] not in self._block_types:
                    click.echo('Must be one of ' + str(self._block_types))

    if self._info['lang'] is None:
        with SequenceCompleter(self.language_candidates):
            while self._info['lang'] not in self.language_candidates:
                self._info['lang'] = input("Language (python/cpp): ")
    if self._info['lang'] == 'c++':
        self._info['lang'] = 'cpp'

    click.echo("Language: {}".format({'cpp': 'C++', 'python': 'Python'}[self._info['lang']]))

    if ((self._skip_subdirs['lib'] and self._info['lang'] == 'cpp')
            or (self._skip_subdirs['python'] and self._info['lang'] == 'python')):
        raise ModToolException('Missing or skipping relevant subdir.')

    if self._info['blockname'] is None:
        self._info['blockname'] = input("Enter name of block/code (without module name prefix): ")
    if not re.match('[a-zA-Z0-9_]+', self._info['blockname']):
        raise ModToolException('Invalid block name.')
    click.echo("Block/code identifier: " + self._info['blockname'])

    self._info['fullblockname'] = self._info['modname'] + '_' + self._info['blockname']

    if not self._license_file:
        if self._info['copyrightholder'] is None:
            self._info['copyrightholder'] = '<+YOU OR YOUR COMPANY+>'
        elif self._info['is_component']:
            click.echo("For GNU Radio components the FSF is added as copyright holder")

    self._info['license'] = self.setup_choose_license()

    if self._info['arglist'] is not None:
        self._info['arglist'] = input('Enter valid argument list, including default arguments: ')

    if not (self._info['blocktype'] in ('noblock') or self._skip_subdirs['python']):
        if self._add_py_qa is None:
            self._add_py_qa = ask_yes_no('Add Python QA code?', True)

    if self._info['lang'] == 'cpp':
        if self._add_cc_qa is None:
            self._add_cc_qa = ask_yes_no('Add C++ QA code?', not self._add_py_qa)

    if self._info['version'] == 'autofoo' and not self._skip_cmakefiles:
        click.echo("Warning: Autotools modules are not supported. ",
                   "Files will be created, but Makefiles will not be edited.")
        self._skip_cmakefiles = True

    run(self)
