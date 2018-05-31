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
""" Disable blocks module """

import sys
from types import SimpleNamespace
import click

from .modtool_base import common_params, block_name
from gnuradio.modtool.core.modtool_disable import ModToolDisable
from gnuradio.modtool.core.modtool_base import ModToolException


@click.command('disable', short_help=ModToolDisable().description)
@ModTool.common_params
@ModTool.block_name
def cli(**kwargs):
    """Disable a block (comments out CMake entries for files)"""
    args = SimpleNamespace(**kwargs)
    try:
        ModToolDisable().run(args)
    except ModToolException as err:
        click.echo(err, file=sys.stderr)
        exit(1)