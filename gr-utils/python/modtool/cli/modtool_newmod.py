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

import re
import os
from types import SimpleNamespace

import click

from gnuradio import gr
from gnuradio.modtool.core.modtool_base import ModToolException
from gnuradio.modtool.core.modtool_newmod import ModToolNewModule
from .modtool_base import common_params, run


@click.command('newmod', short_help=ModToolNewModule().description)
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
    options = SimpleNamespace(**kwargs)
    self = ModToolNewModule()
    self._cli = True

    self._info['modname'] = options.module_name
    if self._info['modname'] is None:
        if options.module_name:
            self._info['modname'] = options.module_name
        else:
            self._info['modname'] = input('Name of the new module: ')
    if not re.match('[a-zA-Z0-9_]+$', self._info['modname']):
        raise ModToolException('Invalid module name.')
    self._dir = options.directory
    if self._dir == '.':
        self._dir = './gr-{}'.format(self._info['modname'])
    else:
        self._dir = self._dir + '/gr-{}'.format(self._info['modname'])
    try:
        os.stat(self._dir)
    except OSError:
        pass # This is what should happen
    else:
        raise ModToolException('The given directory exists.')
    if options.srcdir is None:
        options.srcdir = '/usr/local/share/gnuradio/modtool/gr-newmod'
    self._srcdir = gr.prefs().get_string('modtool', 'newmod_path', options.srcdir)
    if not os.path.isdir(self._srcdir):
        raise ModToolException('Could not find gr-newmod source dir.')
    self.options = options
    self._setup_scm(mode='new')

    run(self, options)
