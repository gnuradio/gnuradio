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
""" Module to rename blocks """

import re
import sys

import click

from .modtool_base import common_params, block_name, run
from gnuradio.modtool.core.modtool_rename import ModToolRename


@click.command('rename', short_help=ModToolRename().description)
@common_params
@block_name
@click.argument('new-name', metavar="NEW-BLOCK-NAME", nargs=1, required=False)
def cli(**kwargs):
    """
    \b
    Rename a block inside a module.

    The argument NEW-BLOCK-NAME is the new name of the block.
    """
    run(ModToolRename, **kwargs)
