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

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import re
import os

import click

from gnuradio import gr
from ..core import ModToolNewModule
from .base import common_params, run, cli_input, ModToolException


@click.command('newmod', short_help=ModToolNewModule.description)
@click.option('--srcdir',
              help="Source directory for the module template.")
@common_params
@click.argument('module_name', metavar="MODULE-NAME", nargs=1, required=False)
def cli(**kwargs):
    """
    \b
    Create a new out-of-tree module

    The argument MODULE-NAME is the name of the module to be added.
    """
    kwargs['cli'] = True
    self = ModToolNewModule(**kwargs)
    get_modname(self)
    self.dir = os.path.join(self.dir, 'gr-{}'.format(self.info['modname']))
    try:
        os.stat(self.dir)
    except OSError:
        pass # This is what should happen
    else:
        raise ModToolException('The given directory exists.')
    if self.srcdir is None:
        self.srcdir = '/usr/local/share/gnuradio/modtool/templates/gr-newmod'
    self.srcdir = gr.prefs().get_string('modtool', 'newmod_path', self.srcdir)
    if not os.path.isdir(self.srcdir):
        raise ModToolException('Could not find gr-newmod source dir.')
    run(self)

def get_modname(self):
    """ Get the name of the new module to be added """
    if self.info['modname'] is None:
        while not self.info['modname'] or self.info['modname'].isspace():
            self.info['modname'] = cli_input('Name of the new module: ')
    if not re.match('[a-zA-Z0-9_]+$', self.info['modname']):
        raise ModToolException('Invalid module name.')
