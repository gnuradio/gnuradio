#
# Copyright 2013, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Base class for the modules """


import os
import re
import glob
import logging
import itertools

from types import SimpleNamespace
from gnuradio import gr
from ..tools import get_modname, SCMRepoFactory

logger = logging.getLogger('gnuradio.modtool')


def get_block_candidates(modname: str = None,
                         skip_lib: bool = False, skip_include: bool = False,
                         skip_python: bool = False, skip_grc: bool = False):
    """ Returns a list of all possible blocknames """
    block_candidates = []
    if not skip_lib:
        cpp_filters = ["*.cc", "*.cpp"]
        for ftr in cpp_filters:
            block_candidates.append(os.path.splitext(x)[0].split("_impl")[0]
                                    for x in glob.glob1("lib", ftr)
                                    if not (x.startswith("qa_") or x.startswith("test_")))
    if not skip_include:
        cpp_header_glob = (glob.glob("include/gnuradio/*/*.h") if modname is None
                           else glob.glob1(f"include/gnuradio/{modname}", "*.h"))
        block_candidates.append(os.path.splitext(os.path.split(x)[-1])[0]
                                for x in cpp_header_glob
                                if x != "api.h")
    if not skip_python:
        python_glob = (glob.glob("python/*/*.py") if modname is None
                       else glob.glob1(f"python/{modname}", "*.py"))
        block_candidates.append(os.path.splitext(os.path.split(x)[-1])[0]
                                for x in python_glob
                                if not (x.startswith("qa_") or
                                        x.startswith("build") or
                                        x == "__init__.py"))
    if not (skip_grc or modname is None):
        block_candidates.append(x.split(".block.yml")[0].split(f"{modname}_")[-1]
                                for x in glob.glob1("grc", "*.block.yml"))
    return set(itertools.chain(*block_candidates))


class ModToolException(Exception):
    """ Standard exception for modtool classes. """
    pass


def validate_name(kind, name):
    """ Checks that block, module etc names are alphanumeric. """
    if not re.fullmatch('[a-zA-Z0-9_]+', name):
        raise ModToolException(
            "Invalid {} name '{}': names can only contain letters, numbers and underscores".format(kind, name))


class ModTool(object):
    """ Base class for all modtool command classes. """
    name = 'base'
    description = None

    def __init__(self, blockname=None, module_name=None, **kwargs):
        # List subdirs where stuff happens
        self._subdirs = ['lib', 'include', 'python', 'grc']
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
        self.skip_subdirs['pybind'] = kwargs.get('skip_pybind', False)
        self.skip_subdirs['grc'] = kwargs.get('skip_grc', False)
        self._scm = kwargs.get('scm_mode',
                               gr.prefs().get_string('modtool', 'scm_mode', 'no'))
        if not self.cli:
            logging.basicConfig(level=logging.ERROR, format='%(message)s')
            self.info['yes'] = True
        else:
            self.info['yes'] = kwargs.get('yes', False)
            from ..cli import setup_cli_logger
            setup_cli_logger(logger)

        if not type(self).__name__ in ['ModToolInfo', 'ModToolNewModule']:
            if self.cli:
                self._validate()

    def _validate(self):
        """ Validates the arguments """
        if not isinstance(self.skip_subdirs['lib'], bool):
            raise ModToolException('Expected a boolean value for skip_lib')
        if not isinstance(self.skip_subdirs['pybind'], bool):
            raise ModToolException('Expected a boolean value for skip_pybind')
        if not isinstance(self.skip_subdirs['python'], bool):
            raise ModToolException('Expected a boolean value for skip_python')
        if not isinstance(self.skip_subdirs['grc'], bool):
            raise ModToolException('Expected a boolean value for skip_grc')
        self._assign()

    def _assign(self):
        if not self._check_directory(self.dir):
            raise ModToolException(
                'No GNU Radio module found in the given directory.')
        if self.info['modname'] is None:
            self.info['modname'] = get_modname()
        if self.info['modname'] is None:
            raise ModToolException(
                'No GNU Radio module found in the given directory.')
        if self.info['version'] == '36' and (
                os.path.isdir(os.path.join('include', self.info['modname'])) or
                os.path.isdir(os.path.join(
                    'include', 'gnuradio', self.info['modname']))
        ):
            self.info['version'] = '37'
        if not os.path.isfile(os.path.join('cmake', 'Modules', 'FindCppUnit.cmake')):
            self.info['version'] = '38'
            if os.path.isdir(os.path.join('include', 'gnuradio', self.info['modname'])):
                self.info['version'] = '310'
        if self.skip_subdirs['lib'] or not self.has_subdirs['lib']:
            self.skip_subdirs['lib'] = True
        if not self.has_subdirs['python']:
            self.skip_subdirs['python'] = True
        # if not self.has_subdirs['pybind']:
        #     self.skip_subdirs['pybind'] = True
        if not self.has_subdirs['grc']:
            self.skip_subdirs['grc'] = True

        self._setup_files()
        self._setup_scm()

    def _setup_files(self):
        """ Initialise the self._file[] dictionary """
        self.info['pydir'] = 'python'
        if os.path.isdir(os.path.join('python', self.info['modname'])):
            self.info['pydir'] = os.path.join('python', self.info['modname'])
        self._file['qalib'] = os.path.join(
            'lib', f'qa_{self.info["modname"]}.cc')
        self._file['pyinit'] = os.path.join(self.info['pydir'], '__init__.py')
        self._file['cmlib'] = os.path.join('lib', 'CMakeLists.txt')
        self._file['cmgrc'] = os.path.join('grc', 'CMakeLists.txt')
        self._file['cmpython'] = os.path.join(
            self.info['pydir'], 'CMakeLists.txt')
        self._file['cmpybind'] = os.path.join(
            self.info['pydir'], 'bindings', 'CMakeLists.txt')
        self._file['ccpybind'] = os.path.join(
            self.info['pydir'], 'bindings', 'python_bindings.cc')
        if self.info['is_component']:
            self.info['includedir'] = os.path.join(
                'include', 'gnuradio', self.info['modname'])
        elif self.info['version'] in ('37', '38'):
            self.info['includedir'] = os.path.join(
                'include', self.info['modname'])
        elif self.info['version'] in ('310'):
            self.info['includedir'] = os.path.join(
                'include', 'gnuradio', self.info['modname'])
        else:
            self.info['includedir'] = 'include'
        self._file['cminclude'] = os.path.join(
            self.info['includedir'], 'CMakeLists.txt')

    def _setup_scm(self, mode='active'):
        """ Initialize source control management. """
        self.options = SimpleNamespace(scm_mode=self._scm)
        if mode == 'active':
            self.scm = SCMRepoFactory(
                self.options, '.').make_active_scm_manager()
        else:
            self.scm = SCMRepoFactory(
                self.options, '.').make_empty_scm_manager()
        if self.scm is None:
            logger.error("Error: Can't set up SCM.")
            exit(1)

    def _check_directory(self, directory):
        """ Guesses if dir is a valid GNU Radio module directory by looking for
        CMakeLists.txt and at least one of the subdirs lib/, and python/
        Changes the directory, if valid. """
        has_makefile = False
        try:
            files = os.listdir(directory)
            os.chdir(directory)
        except OSError:
            logger.error(f"Can't read or chdir to directory {directory}.")
            return False
        self.info['is_component'] = False
        for f in files:
            if os.path.isfile(f) and f == 'CMakeLists.txt':
                with open(f) as filetext:
                    if re.search(r'find_package\(Gnuradio', filetext.read()) is not None:
                        # Might be 37, check that later
                        self.info['version'] = '36'
                        has_makefile = True
                    elif re.search('GR_REGISTER_COMPONENT', filetext.read()) is not None:
                        # Might be 37, check that later
                        self.info['version'] = '36'
                        self.info['is_component'] = True
                        has_makefile = True
            # TODO search for autofoo
            elif os.path.isdir(f):
                if (f in list(self.has_subdirs.keys())):
                    self.has_subdirs[f] = True
                else:
                    self.skip_subdirs[f] = True
        return bool(has_makefile and (list(self.has_subdirs.values())))

    def run(self):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')
