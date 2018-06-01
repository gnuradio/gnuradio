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

import os
import sys
import re
from types import SimpleNamespace

import click

from .modtool_base import common_params, block_name
from gnuradio.modtool.core.modtool_add import ModToolAdd
from gnuradio.modtool.core.modtool_base import ModToolException


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
@ModTool.common_params
@ModTool.block_name
def cli(**kwargs):
    """Adds a block to the out-of-tree module."""
    args = SimpleNamespace(**kwargs)
    try:
        ModToolAdd().run(args)
    except ModToolException as err:
        click.echo(err, file=sys.stderr)
        exit(1)
