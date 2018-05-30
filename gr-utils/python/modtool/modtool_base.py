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
""" Base class for the modules """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import sys
import re
import functools
from importlib import import_module
from pkg_resources import iter_entry_points
import click
from click_plugins import with_plugins

from gnuradio import gr
from .util_functions import get_modname
from .scm import SCMRepoFactory


class CommandCLI(click.Group):
    """
    This is derived class of the implemented click class
    which overrides some of the functional definitions for external
    plug-in support
    """
    cmd_folder = os.path.abspath(os.path.dirname(__file__))

    def list_commands(self, ctx):
        """
        Lists all the commands available in the modtool directory
        as well as the commands from external plug-ins.
        """
        cmds = []
        for filename in os.listdir(self.cmd_folder):
            if filename.endswith('.py') and filename.startswith('modtool_'):
                cmds.append(filename[8:-3])
        cmds.remove('base')
        cmds += self.commands
        return sorted(cmds)

    def get_command(self, ctx, cmd_name):
        """
        Returns a command object if it exists. The existing in-tree ModTool
        command is the priority over the same external plug-in command.
        """
        try:
            if sys.version_info[0] == 2:
                cmd_name = cmd_name.encode('ascii', 'replace')
            mod = import_module('gnuradio.modtool.modtool_' + cmd_name)
        except ImportError:
            return self.commands.get(cmd_name)
        return mod.cli


class ModToolException(BaseException):
    """ Standard exception for modtool classes. """
    pass


class ModTool(object):
    """ Base class for all modtool command classes. """
    name = 'base'
    description = None

    def __init__(self):
        self._subdirs = ['lib', 'include', 'python', 'swig', 'grc'] # List subdirs where stuff happens
        self._has_subdirs = {}
        self._skip_subdirs = {}
        self._info = {}
        self._file = {}
        for subdir in self._subdirs:
            self._has_subdirs[subdir] = False
            self._skip_subdirs[subdir] = False
        self._dir = None

    block_name = click.argument('blockname', nargs=1, required=False, metavar="BLOCK_NAME")

    def common_params(func):
        """ Common parameters for various modules"""
        @click.option('-d', '--directory', default='.',
                      help="Base directory of the module. Defaults to the cwd.")
        @click.option('--skip-lib', is_flag=True,
                      help="Don't do anything in the lib/ subdirectory.")
        @click.option('--skip-swig', is_flag=True,
                      help="Don't do anything in the swig/ subdirectory.")
        @click.option('--skip-python', is_flag=True,
                      help="Don't do anything in the python/ subdirectory.")
        @click.option('--skip-grc', is_flag=True,
                      help="Don't do anything in the grc/ subdirectory.")
        @click.option('--scm-mode', type=click.Choice(['yes', 'no', 'auto']),
                      default=gr.prefs().get_string('modtool', 'scm_mode', 'no'),
                      help="Use source control management [ yes | no | auto ]).")
        @click.option('-y', '--yes', is_flag=True,
                      help="Answer all questions with 'yes'. " +
                      "This can overwrite and delete your files, so be careful.")
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            """ Decorator that wraps common options """
            return func(*args, **kwargs)
        return wrapper

    def setup(self, options):
        """ Initialise all internal variables, such as the module name etc. """
        self._dir = options.directory
        if not self._check_directory(self._dir):
            raise ModToolException('No GNU Radio module found in the given directory.')
        if hasattr(options, 'module_name') and options.module_name is not None:
            self._info['modname'] = options.module_name
        else:
            self._info['modname'] = get_modname()
        if self._info['modname'] is None:
            raise ModToolException('No GNU Radio module found in the given directory.')
        print("GNU Radio module name identified: " + self._info['modname'])
        if self._info['version'] == '36' and (
                os.path.isdir(os.path.join('include', self._info['modname'])) or
                os.path.isdir(os.path.join('include', 'gnuradio', self._info['modname']))
                ):
            self._info['version'] = '37'
        if options.skip_lib or not self._has_subdirs['lib']:
            self._skip_subdirs['lib'] = True
        if options.skip_python or not self._has_subdirs['python']:
            self._skip_subdirs['python'] = True
        if options.skip_swig or self._get_mainswigfile() is None or not self._has_subdirs['swig']:
            self._skip_subdirs['swig'] = True
        if options.skip_grc or not self._has_subdirs['grc']:
            self._skip_subdirs['grc'] = True
        self._info['blockname'] = options.blockname
        self._setup_files()
        self._info['yes'] = options.yes
        self.options = options
        self._setup_scm()

    def _setup_files(self):
        """ Initialise the self._file[] dictionary """
        if not self._skip_subdirs['swig']:
            self._file['swig'] = os.path.join('swig',   self._get_mainswigfile())
        self._info['pydir'] = 'python'
        if os.path.isdir(os.path.join('python', self._info['modname'])):
            self._info['pydir'] = os.path.join('python', self._info['modname'])
        self._file['qalib']    = os.path.join('lib',    'qa_%s.cc' % self._info['modname'])
        self._file['pyinit']   = os.path.join(self._info['pydir'], '__init__.py')
        self._file['cmlib']    = os.path.join('lib',    'CMakeLists.txt')
        self._file['cmgrc']    = os.path.join('grc',    'CMakeLists.txt')
        self._file['cmpython'] = os.path.join(self._info['pydir'], 'CMakeLists.txt')
        if self._info['is_component']:
            self._info['includedir'] = os.path.join('include', 'gnuradio', self._info['modname'])
        elif self._info['version'] == '37':
            self._info['includedir'] = os.path.join('include', self._info['modname'])
        else:
            self._info['includedir'] = 'include'
        self._file['cminclude'] = os.path.join(self._info['includedir'], 'CMakeLists.txt')
        self._file['cmswig'] = os.path.join('swig', 'CMakeLists.txt')
        self._file['cmfind'] = os.path.join('cmake', 'Modules', 'howtoConfig.cmake')


    def _setup_scm(self, mode='active'):
        """ Initialize source control management. """
        if mode == 'active':
            self.scm = SCMRepoFactory(self.options, '.').make_active_scm_manager()
        else:
            self.scm = SCMRepoFactory(self.options, '.').make_empty_scm_manager()
        if self.scm is None:
            print("Error: Can't set up SCM.")
            exit(1)

    def _check_directory(self, directory):
        """ Guesses if dir is a valid GNU Radio module directory by looking for
        CMakeLists.txt and at least one of the subdirs lib/, python/ and swig/.
        Changes the directory, if valid. """
        has_makefile = False
        try:
            files = os.listdir(directory)
            os.chdir(directory)
        except OSError:
            print("Can't read or chdir to directory %s." % directory)
            return False
        self._info['is_component'] = False
        for f in files:
            if os.path.isfile(f) and f == 'CMakeLists.txt':
                if re.search('find_package\(Gnuradio', open(f).read()) is not None:
                    self._info['version'] = '36' # Might be 37, check that later
                    has_makefile = True
                elif re.search('GR_REGISTER_COMPONENT', open(f).read()) is not None:
                    self._info['version'] = '36' # Might be 37, check that later
                    self._info['is_component'] = True
                    has_makefile = True
            # TODO search for autofoo
            elif os.path.isdir(f):
                if (f in list(self._has_subdirs.keys())):
                    self._has_subdirs[f] = True
                else:
                    self._skip_subdirs[f] = True
        return bool(has_makefile and (list(self._has_subdirs.values())))

    def _get_mainswigfile(self):
        """ Find out which name the main SWIG file has. In particular, is it
            a MODNAME.i or a MODNAME_swig.i? Returns None if none is found. """
        modname = self._info['modname']
        swig_files = (modname + '.i',
                      modname + '_swig.i')
        for fname in swig_files:
            if os.path.isfile(os.path.join(self._dir, 'swig', fname)):
                return fname
        return None

    def run(self, options):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')


### COMMAND LINE INTERFACE ###
@with_plugins(iter_entry_points('gnuradio.modtool.plugins'))
@click.command(cls=CommandCLI,
               epilog='Manipulate with GNU Radio modules source code tree. ' +
               'Call it without options to run specified command interactively')
def cli():
    """A tool for editing GNU Radio out-of-tree modules."""
    pass
