""" Create a whole new out-of-tree module """

import shutil
import os
import re
from optparse import OptionGroup
from modtool_base import ModTool

### New out-of-tree-mod module ###############################################
class ModToolNewModule(ModTool):
    """ Create a new out-of-tree module """
    name = 'newmod'
    aliases = ('nm', 'create')
    def __init__(self):
        ModTool.__init__(self)

    def setup_parser(self):
        " Initialise the option parser for 'gr_modtool.py newmod' "
        parser = ModTool.setup_parser(self)
        parser.usage = '%prog rm [options]. \n Call %prog without any options to run it interactively.'
        ogroup = OptionGroup(parser, "New out-of-tree module options")
        parser.add_option_group(ogroup)
        return parser

    def setup(self):
        (options, self.args) = self.parser.parse_args()
        self._info['modname'] = options.module_name
        if self._info['modname'] is None:
            if len(self.args) >= 2:
                self._info['modname'] = self.args[1]
            else:
                self._info['modname'] = raw_input('Name of the new module: ')
        if not re.match('[a-zA-Z0-9_]+', self._info['modname']):
            print 'Invalid module name.'
            exit(2)
        self._dir = options.directory
        if self._dir == '.':
            self._dir = './gr-%s' % self._info['modname']
        print 'Module directory is "%s".' % self._dir
        try:
            os.stat(self._dir)
        except OSError:
            pass # This is what should happen
        else:
            print 'The given directory exists.'
            exit(2)

    def run(self):
        """
        * Copy the example dir recursively
        * Open all files, rename howto and HOWTO to the module name
        * Rename files and directories that contain the word howto
        """
        print "Creating out-of-tree module in %s..." % self._dir
        try:
            shutil.copytree('/home/braun/.usrlocal/share/gnuradio/modtool/gr-newmod', self._dir)
            os.chdir(self._dir)
        except OSError:
            print 'Could not create directory %s. Quitting.' % self._dir
            exit(2)
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
        print "Use 'gr_modtool add' to add a new block to this currently empty module."

