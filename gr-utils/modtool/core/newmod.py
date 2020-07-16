#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Create a whole new out-of-tree module """


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
        self.dir = os.path.join(self.directory, f'gr-{self.info["modname"]}')
        if self.srcdir is None:
            self.srcdir = os.path.join(gr.prefix(),'share','gnuradio','modtool','templates','gr-newmod')

    def validate(self):
        """ Validates the arguments """
        if not self.info['modname']:
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
            raise ModToolException('Could not find gr-newmod source dir \'' + self.srcdir + '\'')

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
        logger.info(f"Creating out-of-tree module in {self.dir}...")
        try:
            shutil.copytree(self.srcdir, self.dir)
            try:
              shutil.copyfile(os.path.join(gr.prefix(), 'share', 'gnuradio', 'clang-format.conf'),
                              os.path.join(self.dir, '.clang-format'))
            except FileNotFoundError as e:
              logger.info(f'Failed to copy .clang-format: {e}')
            os.chdir(self.dir)
        except OSError:
            raise ModToolException(f'Could not create directory {self.dir}.')
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
