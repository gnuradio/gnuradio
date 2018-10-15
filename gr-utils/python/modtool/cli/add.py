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

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import re

import click

from ..core import ModToolAdd
from ..tools import SequenceCompleter, ask_yes_no
from .base import common_params, block_name, run, cli_input, ModToolException


@click.command('add')
@click.option('-t', '--block-type', type=click.Choice(ModToolAdd.block_types),
              help="One of {}.".format(', '.join(ModToolAdd.block_types)))
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
    self = ModToolAdd(**kwargs)
    click.secho("GNU Radio module name identified: " + self.info['modname'], fg='green')
    get_blocktype(self)
    get_lang(self)
    click.secho("Language: {}".format({'cpp': 'C++', 'python': 'Python'}[self.info['lang']]), fg='green')
    if ((self.skip_subdirs['lib'] and self.info['lang'] == 'cpp')
            or (self.skip_subdirs['python'] and self.info['lang'] == 'python')):
        raise ModToolException('Missing or skipping relevant subdir.')
    get_blockname(self)
    click.secho("Block/code identifier: " + self.info['blockname'], fg='green')
    self.info['fullblockname'] = self.info['modname'] + '_' + self.info['blockname']
    if not self.license_file:
        get_copyrightholder(self)
    self.info['license'] = self.setup_choose_license()
    get_arglist(self)
    get_py_qa(self)
    get_cpp_qa(self)
    if self.info['version'] == 'autofoo' and not self.skip_cmakefiles:
        click.secho("Warning: Autotools modules are not supported. "+
                    "Files will be created, but Makefiles will not be edited.",
                    fg='yellow')
        self.skip_cmakefiles = True
    run(self)

def get_blocktype(self):
    """ Get the blocktype of the block to be added """
    if self.info['blocktype'] is None:
        click.secho(str(self.block_types), fg='yellow')
        with SequenceCompleter(self.block_types):
            while self.info['blocktype'] not in self.block_types:
                self.info['blocktype'] = cli_input("Enter block type: ")
                if self.info['blocktype'] not in self.block_types:
                    click.secho('Must be one of ' + str(self.block_types), fg='yellow')

def get_lang(self):
    """ Get the Programming Language of the block to be added """
    if self.info['lang'] is None:
        with SequenceCompleter(self.language_candidates):
            while self.info['lang'] not in self.language_candidates:
                self.info['lang'] = cli_input("Language (python/cpp): ")
    if self.info['lang'] == 'c++':
        self.info['lang'] = 'cpp'

def get_blockname(self):
    """ Get the blockname"""
    if self.info['blockname'] is None:
        while not self.info['blockname'] or self.info['blockname'].isspace():
            self.info['blockname'] = cli_input("Enter name of block/code (without module name prefix): ")
    if not re.match('[a-zA-Z0-9_]+', self.info['blockname']):
        raise ModToolException('Invalid block name.')

def get_copyrightholder(self):
    """ Get the copyrightholder of the block to be added """
    if self.info['copyrightholder'] is None:
        self.info['copyrightholder'] = '<+YOU OR YOUR COMPANY+>'
    elif self.info['is_component']:
        click.secho("For GNU Radio components the FSF is added as copyright holder",
                    fg='cyan')

def get_arglist(self):
    """ Get the argument list of the block to be added """
    if self.info['arglist'] is not None:
        self.info['arglist'] = cli_input('Enter valid argument list, including default arguments: ')

def get_py_qa(self):
    """ Get a boolean value for addition of py_qa """
    if not (self.info['blocktype'] in ('noblock') or self.skip_subdirs['python']):
        if self.add_py_qa is None:
            self.add_py_qa = ask_yes_no(click.style('Add Python QA code?', fg='cyan'), True)

def get_cpp_qa(self):
    """ Get a boolean value for addition of cpp_qa """
    if self.info['lang'] == 'cpp':
        if self.add_cc_qa is None:
            self.add_cc_qa = ask_yes_no(click.style('Add C++ QA code?', fg='cyan'),
                                        not self.add_py_qa)
