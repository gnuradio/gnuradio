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
""" Base class for the modules """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import glob
import logging
import itertools
from types import SimpleNamespace

from gnuradio import gr
from ..tools import get_modname, SCMRepoFactory
from ..cli import setup_cli_logger

logger = logging.getLogger('gnuradio.modtool')


def get_block_candidates():
    """ Returns a list of all possible blocknames """
    block_candidates = []
    cpp_filters = ["*.cc", "*.cpp"]
    cpp_blocks = []
    for ftr in cpp_filters:
        cpp_blocks += [x for x in glob.glob1("lib", ftr) if not (x.startswith('qa_') or
                       x.startswith('test_'))]
    python_blocks = [x for x in glob.glob1("python", "*.py") if not (x.startswith('qa_') or
                     x.startswith('build') or x.startswith('__init__'))]
    for block in itertools.chain(cpp_blocks, python_blocks):
        block = os.path.splitext(block)[0]
        block = block.split('_impl')[0]
        block_candidates.append(block)
    return block_candidates


class ModToolException(Exception):
    """ Standard exception for modtool classes. """
    pass


class ModTool(object):
    """ Base class for all modtool command classes. """
    name = 'base'
    description = None

    def __init__(self, blockname=None, module_name=None, **kwargs):
        # List subdirs where stuff happens
        self._subdirs = ['lib', 'include', 'python', 'swig', 'grc']
        self.has_subdirs = {}
        self.skip_subdirs = {}
        self.info = {}
        self._file = {}
        for subdir in self._subdirs:
            self.has_subdirs[subdir] = False
            self.skip_subdirs[subdir] = False
        self.info['blockname'] = blockname
        self.info['modname'] = module_name
        self.cli = kwargs.get('cli', False)
        self.dir = kwargs.get('directory', '.')
        self.skip_subdirs['lib'] = kwargs.get('skip_lib', False)
        self.skip_subdirs['python'] = kwargs.get('skip_python', False)
        self.skip_subdirs['swig'] = kwargs.get('skip_swig', False)
        self.skip_subdirs['grc'] = kwargs.get('skip_grc', False)
        self._scm = kwargs.get('scm_mode',
                               gr.prefs().get_string('modtool', 'scm_mode', 'no'))
        if not self.cli:
            logging.basicConfig(level=logging.ERROR, format='%(message)s')
            self.info['yes'] = True
        else:
            self.info['yes'] = kwargs.get('yes', False)
            setup_cli_logger(logger)

        if not type(self).__name__ in ['ModToolInfo', 'ModToolNewModule']:
            if self.cli:
                self._validate()

    def _validate(self):
        """ Validates the arguments """
        if not isinstance(self.skip_subdirs['lib'], bool):
            raise ModToolException('Expected a boolean value for skip_lib')
        if not isinstance(self.skip_subdirs['swig'], bool):
            raise ModToolException('Expected a boolean value for skip_swig')
        if not isinstance(self.skip_subdirs['python'], bool):
            raise ModToolException('Expected a boolean value for skip_python')
        if not isinstance(self.skip_subdirs['grc'], bool):
            raise ModToolException('Expected a boolean value for skip_grc')
        self._assign()

    def _assign(self):
        if not self._check_directory(self.dir):
            raise ModToolException('No GNU Radio module found in the given directory.')
        if self.info['modname'] is None:
            self.info['modname'] = get_modname()
        if self.info['modname'] is None:
            raise ModToolException('No GNU Radio module found in the given directory.')
        if self.info['version'] == '36' and (
                os.path.isdir(os.path.join('include', self.info['modname'])) or
                os.path.isdir(os.path.join('include', 'gnuradio', self.info['modname']))
                ):
            self.info['version'] = '37'
        if not os.path.isfile(os.path.join('cmake', 'Modules', 'FindCppUnit.cmake')):
            self.info['version'] = '38'
        if self.skip_subdirs['lib'] or not self.has_subdirs['lib']:
            self.skip_subdirs['lib'] = True
        if not self.has_subdirs['python']:
            self.skip_subdirs['python'] = True
        if self._get_mainswigfile() is None or not self.has_subdirs['swig']:
            self.skip_subdirs['swig'] = True
        if not self.has_subdirs['grc']:
            self.skip_subdirs['grc'] = True

        self._setup_files()
        self._setup_scm()

    def _setup_files(self):
        """ Initialise the self._file[] dictionary """
        if not self.skip_subdirs['swig']:
            self._file['swig'] = os.path.join('swig',   self._get_mainswigfile())
        self.info['pydir'] = 'python'
        if os.path.isdir(os.path.join('python', self.info['modname'])):
            self.info['pydir'] = os.path.join('python', self.info['modname'])
        self._file['qalib']    = os.path.join('lib',    'qa_{}.cc'.format(self.info['modname']))
        self._file['pyinit']   = os.path.join(self.info['pydir'], '__init__.py')
        self._file['cmlib']    = os.path.join('lib',    'CMakeLists.txt')
        self._file['cmgrc']    = os.path.join('grc',    'CMakeLists.txt')
        self._file['cmpython'] = os.path.join(self.info['pydir'], 'CMakeLists.txt')
        if self.info['is_component']:
            self.info['includedir'] = os.path.join('include', 'gnuradio', self.info['modname'])
        elif self.info['version'] in ('37', '38'):
            self.info['includedir'] = os.path.join('include', self.info['modname'])
        else:
            self.info['includedir'] = 'include'
        self._file['cminclude'] = os.path.join(self.info['includedir'], 'CMakeLists.txt')
        self._file['cmswig'] = os.path.join('swig', 'CMakeLists.txt')
        self._file['cmfind'] = os.path.join('cmake', 'Modules', 'howtoConfig.cmake')


    def _setup_scm(self, mode='active'):
        """ Initialize source control management. """
        self.options = SimpleNamespace(scm_mode = self._scm)
        if mode == 'active':
            self.scm = SCMRepoFactory(self.options, '.').make_active_scm_manager()
        else:
            self.scm = SCMRepoFactory(self.options, '.').make_empty_scm_manager()
        if self.scm is None:
            logger.error("Error: Can't set up SCM.")
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
            logger.error("Can't read or chdir to directory {}.".format(directory))
            return False
        self.info['is_component'] = False
        for f in files:
            if os.path.isfile(f) and f == 'CMakeLists.txt':
                with open(f) as filetext:
                    if re.search('find_package\(Gnuradio', filetext.read()) is not None:
                        self.info['version'] = '36' # Might be 37, check that later
                        has_makefile = True
                    elif re.search('GR_REGISTER_COMPONENT', filetext.read()) is not None:
                        self.info['version'] = '36' # Might be 37, check that later
                        self.info['is_component'] = True
                        has_makefile = True
            # TODO search for autofoo
            elif os.path.isdir(f):
                if (f in list(self.has_subdirs.keys())):
                    self.has_subdirs[f] = True
                else:
                    self.skip_subdirs[f] = True
        return bool(has_makefile and (list(self.has_subdirs.values())))

    def _get_mainswigfile(self):
        """ Find out which name the main SWIG file has. In particular, is it
            a MODNAME.i or a MODNAME_swig.i? Returns None if none is found. """
        modname = self.info['modname']
        swig_files = (modname + '.i',
                      modname + '_swig.i')
        for fname in swig_files:
            if os.path.isfile(os.path.join(self.dir, 'swig', fname)):
                return fname
        return None

    def run(self):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')
