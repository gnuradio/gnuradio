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
""" Create a whole new out-of-tree module """

import sys

import click

from .modtool_base import common_params, block_name, run
from gnuradio.modtool.core.modtool_newmod import ModToolNewModule


@click.command('newmod', short_help=ModToolNewModule().description)
@click.option('--srcdir',
              help="Source directory for the module template.")
@common_params
@click.argument('module_name', metavar="MODULE-NAME", nargs=1, required=False)
def cli(**kwargs):
    """
    \b
    Create a new out-of-tree module

    The argument MODULE-NAME overrides the current module's name (normally is autodetected).
    """
    run(ModToolNewModule, **kwargs)
