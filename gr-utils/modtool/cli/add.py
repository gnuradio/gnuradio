#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to add new blocks """


import os
import re
import getpass

import click

from ..core import ModToolAdd, validate_name
from ..tools import SequenceCompleter, ask_yes_no
from .base import common_params, block_name, run, cli_input, ModToolException


class BlockType(click.ParamType):
    name = "blocktype"

    def convert(self, value, param, ctx):
        if value in ModToolAdd.block_types:
            return value
        else:
            self.fail(f"Invalid blocktype: '{value}'")

    def shell_complete(self, ctx, param, incomplete: str):
        return [
            click.shell_completion.CompletionItem(name, help=description)
            for name, description in ModToolAdd.block_types.items()
            if name.startswith(incomplete)
        ]


@click.command('add')
@click.option('-t', '--block-type', type=BlockType(),
              help=f"One of {', '.join(ModToolAdd.block_types)}.")
@click.option('--license-file',
              type=click.Path(file_okay=True, dir_okay=False, readable=True),
              help="File containing the license header for every source code file.")
@click.option('--copyright',
              help="Name of the copyright holder (you or your company) MUST be a quoted string.")
@click.option('--argument-list', default=None,
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
@click.argument("blockname", nargs=1, required=False, metavar="BLOCK_NAME")
def cli(**kwargs):
    """Adds a block to the out-of-tree module."""
    kwargs['cli'] = True
    self = ModToolAdd(**kwargs)
    click.secho("GNU Radio module name identified: " +
                self.info['modname'], fg='green')
    get_blockname(self)
    get_blocktype(self)
    get_lang(self)
    info_lang = {'cpp': 'C++', 'python': 'Python'}[self.info['lang']]
    click.secho(f"Language: {info_lang}", fg='green')
    if ((self.skip_subdirs['lib'] and self.info['lang'] == 'cpp') or
            (self.skip_subdirs['python'] and self.info['lang'] == 'python')):
        raise ModToolException('Missing or skipping relevant subdir.')
    click.secho("Block/code identifier: " + self.info['blockname'], fg='green')
    if not self.license_file:
        get_copyrightholder(self)
    self.info['license'] = self.setup_choose_license()
    get_arglist(self)
    get_py_qa(self)
    get_cpp_qa(self)
    if self.info['version'] == 'autofoo' and not self.skip_cmakefiles:
        click.secho("Warning: Autotools modules are not supported. " +
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
                    click.secho('Must be one of ' +
                                str(self.block_types), fg='yellow')


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
    if not self.info['blockname'] or self.info['blockname'].isspace():
        while not self.info['blockname'] or self.info['blockname'].isspace():
            self.info['blockname'] = cli_input(
                "Enter name of block/code (without module name prefix): ")
    validate_name('block', self.info['blockname'])
    self.info['fullblockname'] = self.info['modname'] + \
        '_' + self.info['blockname']
    fname_grc = self.info['fullblockname'] + '.block.yml'
    for block in os.scandir('./grc/'):
        if block.is_file():
            s = block.name
            if s == fname_grc:
                raise ModToolException('Block Already Present.')


def get_copyrightholder(self):
    """ Get the copyrightholder of the block to be added """
    if not self.info['copyrightholder'] or self.info['copyrightholder'].isspace():
        user = getpass.getuser()
        git_user = self.scm.get_gituser()
        if git_user:
            copyright_candidates = (user, git_user, 'GNU Radio')
        else:
            copyright_candidates = (user, 'GNU Radio')
        with SequenceCompleter(copyright_candidates):
            self.info['copyrightholder'] = cli_input(
                "Please specify the copyright holder: ")
            if not self.info['copyrightholder'] or self.info['copyrightholder'].isspace():
                self.info['copyrightholder'] = f'gr-{self.info["modname"]} author'
    elif self.info['is_component']:
        click.secho("For GNU Radio components the FSF is added as copyright holder",
                    fg='cyan')


def get_arglist(self):
    """ Get the argument list of the block to be added """
    if self.info['arglist'] is None:
        self.info['arglist'] = click.prompt(click.style(
            'Enter valid argument list, including default arguments: \n',
            fg='cyan'),
            prompt_suffix='',
            default='',
            show_default=False)


def get_py_qa(self):
    """ Get a boolean value for addition of py_qa """
    if self.add_py_qa is None:
        if not (self.info['blocktype'] in ('noblock') or self.skip_subdirs['python']):
            self.add_py_qa = ask_yes_no(click.style(
                'Add Python QA code?', fg='cyan'), True)
        else:
            self.add_py_qa = False


def get_cpp_qa(self):
    """ Get a boolean value for addition of cpp_qa """
    if self.add_cc_qa is None:
        if self.info['lang'] == 'cpp':
            self.add_cc_qa = ask_yes_no(click.style('Add C++ QA code?', fg='cyan'),
                                        not self.add_py_qa)
        else:
            self.add_cc_qa = False
