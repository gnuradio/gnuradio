#
# Copyright 2013, 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Disable blocks module """


import os
import re
import sys
import logging

from ..tools import CMakeFileEditor
from .base import ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolDisable(ModTool):
    """ Disable block (comments out CMake entries for files) """
    name = 'disable'
    description = 'Disable selected block in module.'

    def __init__(self, blockname=None, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['pattern'] = blockname

    def validate(self):
        """ Validates the arguments """
        ModTool._validate(self)
        if not self.info['pattern'] or self.info['pattern'].isspace():
            raise ModToolException("Invalid pattern!")

    def run(self):
        """ Go, go, go! """
        def _handle_py_qa(cmake, fname):
            """ Do stuff for py qa """
            cmake.comment_out_lines('GR_ADD_TEST.*' + fname)
            self.scm.mark_file_updated(cmake.filename)
            return True

        def _handle_py_mod(cmake, fname):
            """ Do stuff for py extra files """
            try:
                with open(self._file['pyinit']) as f:
                    initfile = f.read()
            except IOError:
                logger.warning(
                    "Could not edit __init__.py, that might be a problem.")
                return False
            pymodname = os.path.splitext(fname)[0]
            initfile = re.sub(
                r'((from|import)\s+\b' + pymodname + r'\b)', r'#\1', initfile)
            with open(self._file['pyinit'], 'w') as f:
                f.write(initfile)
            self.scm.mark_file_updated(self._file['pyinit'])
            return False

        def _handle_cc_qa(cmake, fname):
            """ Do stuff for cc qa """
            if self.info['version'] == '37':
                cmake.comment_out_lines(
                    r'\$\{CMAKE_CURRENT_SOURCE_DIR\}/' + fname)
                fname_base = os.path.splitext(fname)[0]
                # Abusing the CMakeFileEditor...
                ed = CMakeFileEditor(self._file['qalib'])
                ed.comment_out_lines(
                    fr'#include\s+"{fname_base}.h"', comment_str='//')
                ed.comment_out_lines(
                    fr'{fname_base}::suite\(\)', comment_str='//')
                ed.write()
                self.scm.mark_file_updated(self._file['qalib'])
            elif self.info['version'] in ['38', '310']:
                fname_qa_cc = f'qa_{self.info["blockname"]}.cc'
                cmake.comment_out_lines(fname_qa_cc)
            elif self.info['version'] == '36':
                cmake.comment_out_lines('add_executable.*' + fname)
                cmake.comment_out_lines(
                    'target_link_libraries.*' + os.path.splitext(fname)[0])
                cmake.comment_out_lines(
                    'GR_ADD_TEST.*' + os.path.splitext(fname)[0])
            self.scm.mark_file_updated(cmake.filename)
            return True

        # This portion will be covered by the CLI
        if not self.cli:
            self.validate()
        else:
            from ..cli import cli_input
        # List of special rules: 0: subdir, 1: filename re match, 2: callback
        special_treatments = (
            ('python', r'qa.+py$', _handle_py_qa),
            ('python', r'^(?!qa).+py$', _handle_py_mod),
            ('lib', r'qa.+\.cc$', _handle_cc_qa)
        )
        for subdir in self._subdirs:
            if self.skip_subdirs[subdir]:
                continue
            if self.info['version'] in ('37', '38') and subdir == 'include':
                subdir = f'include/{self.info["modname"]}'
            try:
                cmake = CMakeFileEditor(os.path.join(subdir, 'CMakeLists.txt'))
            except IOError:
                continue
            logger.info(f"Traversing {subdir}...")
            filenames = []
            if self.info['blockname']:
                if subdir == 'python':
                    blockname_pattern = f"^(qa_)?{self.info['blockname']}.py$"
                elif subdir == 'python/bindings':
                    blockname_pattern = f"^{self.info['blockname']}_python.cc$"
                elif subdir == 'python/bindings/docstrings':
                    blockname_pattern = f"^{self.info['blockname']}_pydoc_template.h$"
                elif subdir == 'lib':
                    blockname_pattern = f"^{self.info['blockname']}_impl(\\.h|\\.cc)$"
                elif subdir == self.info['includedir']:
                    blockname_pattern = f"^{self.info['blockname']}.h$"
                elif subdir == 'grc':
                    blockname_pattern = f"^{self.info['modname']}_{self.info['blockname']}.block.yml$"
                if blockname_pattern:
                    filenames = cmake.find_filenames_match(blockname_pattern)
            elif self.info['pattern']:
                filenames = cmake.find_filenames_match(self.info['pattern'])
            yes = self.info['yes']
            for fname in filenames:
                file_disabled = False
                if not yes:
                    ans = cli_input(
                        f"Really disable {fname}? [Y/n/a/q]: ").lower().strip()
                    if ans == 'a':
                        yes = True
                    if ans == 'q':
                        sys.exit(0)
                    if ans == 'n':
                        continue
                for special_treatment in special_treatments:
                    if special_treatment[0] == subdir and re.match(special_treatment[1], fname):
                        file_disabled = special_treatment[2](cmake, fname)
                if not file_disabled:
                    cmake.disable_file(fname)
            cmake.write()
            self.scm.mark_files_updated(
                (os.path.join(subdir, 'CMakeLists.txt'),))
        logger.warning(
            "Careful: 'gr_modtool disable' does not resolve dependencies.")
