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
""" Returns information about a module """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import click

from ..core import ModToolInfo
from .base import common_params, run


@click.command('info')
@click.option('--python-readable', is_flag=True,
              help="Return the output in a format that's easier to read for Python scripts.")
@click.option('--suggested-dirs',
              help="Suggest typical include dirs if nothing better can be detected.")
@common_params
def cli(**kwargs):
    """ Return information about a given module """
    self = ModToolInfo(**kwargs)
    run(self)
