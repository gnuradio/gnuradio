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
""" Automatically create XML bindings for GRC from block code """

import sys
from types import SimpleNamespace
import click

from .modtool_base import common_params, block_name
from gnuradio.modtool.core.modtool_makexml import ModToolMakeXML
from gnuradio.modtool.core.modtool_base import ModToolException


@click.command('makexml', short_help=ModToolMakeXML().description)
@common_params
@block_name
def cli(**kwargs):
    """
    \b
    Make an XML file for GRC block bindings

    Note: This does not work on python blocks
    """
    args = SimpleNamespace(**kwargs)
    try:
        ModToolMakeXML().run(args)
    except ModToolException as err:
        print(err, file=sys.stderr)
        exit(1)
