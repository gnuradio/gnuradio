#
# Copyright 2013 Free Software Foundation, Inc.
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

import shutil
import os
import re
from optparse import OptionGroup
from gnuradio import gr
from modtool_base import ModTool, ModToolException
from scm import SCMRepoFactory

class ModToolNewModule(ModTool):
    """ Create a new out-of-tree module """
    name = 'newmod'
    aliases = ('nm', 'create')
    def __init__(self):
        ModTool.__init__(self)

    def setup_parser(self):
        " Initialise the option parser for 'gr_modtool newmod' "
        parser = ModTool.setup_parser(self)
        parser.usage = '%prog nm [options]. \n Call %prog without any options to run it interactively.'
        ogroup = OptionGroup(parser, "New out-of-tree module options")
        ogroup.add_option("--srcdir", type="string",
                help="Source directory for the module template.")
        parser.add_option_group(ogroup)
        return parser

    def setup(self, options, args):
        # Don't call ModTool.setup(), that assumes an existing module.
        self._info['modname'] = options.module_name
        if self._info['modname'] is None:
            if len(args) >= 2:
                self._info['modname'] = args[1]
            else:
                self._info['modname'] = raw_input('Name of the new module: ')
        if not re.match('[a-zA-Z0-9_]+$', self._info['modname']):
            raise ModToolException('Invalid module name.')
        self._dir = options.directory
        if self._dir == '.':
            self._dir = './gr-%s' % self._info['modname']
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

    def run(self):
        """
        * Copy the example dir recursively
        * Open all files, rename howto and HOWTO to the module name
        * Rename files and directories that contain the word howto
        """
        print "Creating out-of-tree module in %s..." % self._dir,
        try:
            shutil.copytree(self._srcdir, self._dir)
            os.chdir(self._dir)
        except OSError:
            raise ModToolException('Could not create directory %s.' % self._dir)
        for root, dirs, files in os.walk('.'):
            for filename in files:
                f = os.path.join(root, filename)
                s = open(f, 'r').read()
                s = s.replace('howto', self._info['modname'])
                s = s.replace('HOWTO', self._info['modname'].upper())
                open(f, 'w').write(s)
                if filename.find('howto') != -1:
                    os.rename(f, os.path.join(root, filename.replace('howto', self._info['modname'])))
            if os.path.basename(root) == 'howto':
                os.rename(root, os.path.join(os.path.dirname(root), self._info['modname']))
        print "Done."
        if self.scm.init_repo(path_to_repo="."):
            print "Created repository... you might want to commit before continuing."
        print "Use 'gr_modtool add' to add a new block to this currently empty module."

