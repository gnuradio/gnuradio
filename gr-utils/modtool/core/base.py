#
# Copyright 2013, 2018, 2024 Free Software Foundation, Inc.
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

from types import SimpleNamespace
from gnuradio import gr
from ..tools import SCMRepoFactory

logger = logging.getLogger('gnuradio.modtool')


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

    _supported_versions = ["autofoo", "36", "37", "38", "39", "310"]
    _subdir_names = ["lib", "include", "python", "grc"]

    def __init__(self, blockname=None, module_name=None, **kwargs):
        self.subdirs = {}
        self.skip_subdirs = {}
        self.info = {}
        self._file = {}
        for subdir in self._subdir_names:
            self.subdirs[subdir] = None
            self.skip_subdirs[subdir] = kwargs.get(f"skip_{subdir}", False)
        self.skip_pybind = kwargs.get("skip_pybind", False)
        self.info['blockname'] = blockname
        self.info['modname'] = module_name
        self.cli = kwargs.get('cli', False)
        self.dir = kwargs.get('directory', '.')
        self._scm = kwargs.get('scm_mode',
                               gr.prefs().get_string('modtool', 'scm_mode', 'no'))
        if not self.cli:
            logging.basicConfig(level=logging.ERROR, format='%(message)s')
            self.info['yes'] = True
        else:
            self.info['yes'] = kwargs.get('yes', False)
            from ..cli import setup_cli_logger
            setup_cli_logger(logger)

        if type(self).__name__ not in ['ModToolInfo', 'ModToolNewModule']:
            if self.cli:
                self._validate()

    def _validate(self):
        """ Validates the arguments """
        for subdir in self._subdir_names:
            if not isinstance(self.skip_subdirs[subdir], bool):
                raise ModToolException(f"Expected a boolean value for skip_{subdir}")
        self._assign()

    def _assign(self):
        if not self._check_directory(self.dir):
            raise ModToolException(
                'No GNU Radio module found in the given directory.')
        if self.info['modname'] is None:
            raise ModToolException(
                'No GNU Radio module found in the given directory.')
        self._setup_files()
        self._setup_scm()

    def _setup_files(self):
        """ Initialise the self._file[] dictionary """
        if not self.skip_subdirs['python']:
            self.info['pydir'] = pydir = self.subdirs['python']
            self._file['cmpython'] = os.path.join(pydir, 'CMakeLists.txt')
            self._file['pyinit'] = os.path.join(pydir, '__init__.py')

            if not self.skip_pybind:
                self._file['cmpybind'] = os.path.join(pydir, 'bindings', 'CMakeLists.txt')
                self._file['ccpybind'] = os.path.join(pydir, 'bindings', 'python_bindings.cc')

        if not self.skip_subdirs['grc']:
            self._file['cmgrc'] = os.path.join(self.subdirs['grc'], 'CMakeLists.txt')

        if not self.skip_subdirs['lib']:
            self._file['cmlib'] = os.path.join(self.subdirs['lib'], 'CMakeLists.txt')
            # if self.info["version"] == "37":
            self._file['qalib'] = os.path.join(self.subdirs['lib'], f"qa_{self.info['modname']}.cc")

        if not self.skip_subdirs['include']:
            self.info['includedir'] = self.subdirs['include']
            self._file['cminclude'] = os.path.join(self.subdirs['include'], 'CMakeLists.txt')

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

    def _check_directory(self, directory: str):
        """
        Validates the module's directory structure by looking at CMakeLists.txt.
        Changes the working directory if valid.
        """
        try:
            files = os.listdir(directory)
            os.chdir(directory)
        except OSError:
            logger.error(f"Can't read or chdir to directory {directory}.")
            return False

        if "gnuradio.project" in files:
            # Read module name from gnuradio.project
            try:
                with open("gnuradio.project", "r") as f:
                    regexp = r"projectname\s*=\s*([\w\-]+)$"
                    self.info["modname"] = re.search(regexp, f.read(), flags=re.MULTILINE).group(1).strip()
            except (IOError, AttributeError):
                pass

        has_makefile = False
        if "CMakeLists.txt" not in files:
            # TODO search for autofoo
            return False

        self.info['is_component'] = False
        with open("CMakeLists.txt") as cmakelists:
            # TODO use cmake_parser
            cmtext = cmakelists.read()
            if re.search("GR_REGISTER_COMPONENT", cmtext, flags=re.IGNORECASE) is not None:
                # Components are parts of the current version of GNU Radio
                self.info["version"] = self._supported_versions[-1]
                self.info["is_component"] = True
                has_makefile = True
            else:
                gr_package = re.search(
                    (
                        r"find_package\s*\(\s*Gnuradio\s+\"?(?P<version>(\d)(\.\d+)(\.\d+)?)\"?"
                        # r"\s+(?P<required>REQUIRED)?\s+(?P<components>COMPONENTS(\s+\w+)*)?\s*\)"
                    ),
                    cmtext,
                    flags=re.IGNORECASE,
                )
                if gr_package is not None:
                    gr_version = gr_package.group("version")
                    gr_version_parts = gr_version.split(".")
                    self.info["version"] = f"{gr_version_parts[0]}{gr_version_parts[1]}"
                    if self.info["version"] in self._supported_versions:
                        has_makefile = True
                    else:
                        logger.error(f"GNU Radio version {gr_version} is not supported")
                        has_makefile = False

            # Read module name from CMakeLists.txt
            try:
                gr_modname_re = r"gr\-(?P<modname>[\w\-]+)"
                gr_project_re = (
                    rf"GR_REGISTER_COMPONENT\s*\(\s*\"{gr_modname_re}\"\s+ENABLE_"
                    if self.info["is_component"]
                    else rf"project\s*\(\s*{gr_modname_re}\s*"
                )
                modname = re.search(gr_project_re, cmtext, flags=re.MULTILINE | re.IGNORECASE).group("modname").strip()
                # FIXME: maybe not needed?
                modname_trans = {'howto-write-a-block': 'howto'}
                if modname in modname_trans:
                    modname = modname_trans[modname]
                if self.info["modname"] is None:
                    self.info["modname"] = modname
                elif modname != self.info["modname"]:
                    logger.warn(
                        f"Project name in CMakeLists.txt (gr-{modname}) is not "
                        f"the same as in gnuradio.project ({self.info['modname']})"
                    )
            except AttributeError:
                pass

            for subdir in self._subdir_names:
                subdir_added = re.search(rf"add_subdirectory\s*\(\s*(?P<path>{subdir}(\/\w+)*)\s*\)", cmtext)
                if subdir_added is not None:
                    self.subdirs[subdir] = subdir_added.group("path")
                else:
                    self.skip_subdirs[subdir] = True

        return has_makefile and any(subdir is not None for subdir in self.subdirs.values())

    def get_block_candidates(
        self, skip_lib: bool = False, skip_include: bool = False, skip_python: bool = False, skip_grc: bool = False
    ):
        """Return a list of all possible blocknames"""
        block_candidates = set()
        if not (skip_lib or self.skip_subdirs["lib"]):
            cpp_filters = ["*.cc", "*.cpp"]
            for ftr in cpp_filters:
                block_candidates.update(
                    os.path.splitext(os.path.split(x)[-1])[0].removesuffix("_impl")
                    for x in glob.glob1(self.subdirs["lib"], ftr)
                    if not (x.startswith("qa_") or x.startswith("test_"))
                )
        if not (skip_include or self.skip_subdirs["include"]):
            block_candidates.update(
                os.path.splitext(os.path.split(x)[-1])[0]
                for x in glob.glob1(self.subdirs["include"], "*.h")
                if x != "api.h"
            )
        if not (skip_python or self.skip_subdirs["python"]):
            block_candidates.update(
                os.path.splitext(os.path.split(x)[-1])[0]
                for x in glob.glob1(self.subdirs["python"], "*.py")
                if not (x.startswith("qa_") or x.startswith("build") or x == "__init__.py")
            )
        if not (skip_grc or self.skip_subdirs["grc"]) or self.info["modname"] is None:
            block_candidates.update(
                x.removesuffix(".block.yml").removeprefix(f"{self.info["modname"]}_")
                for x in glob.glob1("grc", "*.block.yml")
            )
        return block_candidates

    def run(self):
        """ Override this. """
        raise NotImplementedError('Module implementation missing')
