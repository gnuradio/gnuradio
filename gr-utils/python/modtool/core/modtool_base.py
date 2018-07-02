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
import re
from types import SimpleNamespace

from gnuradio import gr
from .util_functions import get_modname
from .scm import SCMRepoFactory


class ModToolException(Exception):
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

    def setup_args(args):
        options = SimpleNamespace()
        options.directory = args.get('directory', '.')
        options.skip_lib = args.get('skip_lib', False)
        if not isinstance(options.skip_lib, bool):
            raise ModToolException('Expected a boolean value for skip_lib')
        options.skip_swig = args.get('skip_swig', False)
        if not isinstance(options.skip_swig, bool):
            raise ModToolException('Expected a boolean value for skip_swig')
        options.skip_python = args.get('skip_python', False)
        if not isinstance(options.skip_python, bool):
            raise ModToolException('Expected a boolean value for skip_python')
        options.skip_grc = args.get('skip_grc', False)
        if not isinstance(options.skip_grc, bool):
            raise ModToolException('Expected a boolean value for skip_grc')
        options.scm_mode = args.get('scm_mode', 
                                    gr.prefs().get_string('modtool', 'scm_mode', 'no'))
        options.yes = True
        options.blockname = args.get('blockname', None)
        return options

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
        if self._cli:
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
