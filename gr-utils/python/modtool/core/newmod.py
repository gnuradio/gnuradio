#
# Copyright 2013, 2018 Free Software Foundation, Inc.
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

import shutil
import os
import re
import logging

from gnuradio import gr
from ..tools import SCMRepoFactory
from .base import ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolNewModule(ModTool):
    """ Create a new out-of-tree module """
    name = 'newmod'
    description = 'Create new empty module, use add to add blocks.'
    def __init__(self, module_name=None, srcdir=None, **kwargs):
        ModTool.__init__(self, None, module_name, **kwargs)
        # Don't call ModTool._validate(), that assumes an existing module.
        self.srcdir = srcdir
        self.directory = self.dir

    def assign(self):
        self.dir = os.path.join(self.directory, 'gr-{}'.format(self.info['modname']))
        if self.srcdir is None:
            self.srcdir = '/usr/local/share/gnuradio/modtool/templates/gr-newmod'
        self.srcdir = gr.prefs().get_string('modtool', 'newmod_path', self.srcdir)

    def validate(self):
        """ Validates the arguments """
        if self.info['modname'] is None:
            raise ModToolException('Module name not specified.')
        if not re.match('[a-zA-Z0-9_]+$', self.info['modname']):
            raise ModToolException('Invalid module name.')
        try:
            os.stat(self.dir)
        except OSError:
            pass # This is what should happen
        else:
            raise ModToolException('The given directory exists.')
        if not os.path.isdir(self.srcdir):
            raise ModToolException('Could not find gr-newmod source dir.')

    def run(self):
        """
        * Copy the example dir recursively
        * Open all files, rename howto and HOWTO to the module name
        * Rename files and directories that contain the word howto
        """
        # This portion will be covered by the CLI
        if not self.cli:
            self.assign()
            self.validate()
        self._setup_scm(mode='new')
        logger.info("Creating out-of-tree module in {}...".format(self.dir))
        try:
            shutil.copytree(self.srcdir, self.dir)
            os.chdir(self.dir)
        except OSError:
            raise ModToolException('Could not create directory {}.'.format(self.dir))
        for root, dirs, files in os.walk('.'):
            for filename in files:
                f = os.path.join(root, filename)
                with open(f, 'r') as filetext:
                    s = filetext.read()
                s = s.replace('howto', self.info['modname'])
                s = s.replace('HOWTO', self.info['modname'].upper())
                with open(f, 'w') as filetext:
                    filetext.write(s)
                if filename.find('howto') != -1:
                    os.rename(f, os.path.join(root, filename.replace('howto', self.info['modname'])))
            if os.path.basename(root) == 'howto':
                os.rename(root, os.path.join(os.path.dirname(root), self.info['modname']))
        logger.info("Done.")
        if self.scm.init_repo(path_to_repo="."):
            logger.info("Created repository... you might want to commit before continuing.")
        logger.info("Use 'gr_modtool add' to add a new block to this currently empty module.")
