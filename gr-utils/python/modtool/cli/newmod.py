#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
