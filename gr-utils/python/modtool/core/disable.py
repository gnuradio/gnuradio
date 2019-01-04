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
""" Disable blocks module """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import sys
import logging

from ..cli import cli_input
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
            cmake.comment_out_lines('GR_ADD_TEST.*'+fname)
            self.scm.mark_file_updated(cmake.filename)
            return True
        def _handle_py_mod(cmake, fname):
            """ Do stuff for py extra files """
            try:
                with open(self._file['pyinit']) as f:
                    initfile = f.read()
            except IOError:
                logger.warning("Could not edit __init__.py, that might be a problem.")
                return False
            pymodname = os.path.splitext(fname)[0]
            initfile = re.sub(r'((from|import)\s+\b'+pymodname+r'\b)', r'#\1', initfile)
            with open(self._file['pyinit'], 'w') as f:
                f.write(initfile)
            self.scm.mark_file_updated(self._file['pyinit'])
            return False
        def _handle_cc_qa(cmake, fname):
            """ Do stuff for cc qa """
            if self.info['version'] == '37':
                cmake.comment_out_lines(r'\$\{CMAKE_CURRENT_SOURCE_DIR\}/'+fname)
                fname_base = os.path.splitext(fname)[0]
                ed = CMakeFileEditor(self._file['qalib']) # Abusing the CMakeFileEditor...
                ed.comment_out_lines(r'#include\s+"{}.h"'.format(fname_base), comment_str='//')
                ed.comment_out_lines(r'{}::suite\(\)'.format(fname_base), comment_str='//')
                ed.write()
                self.scm.mark_file_updated(self._file['qalib'])
            elif self.info['version'] == '38':
                fname_qa_cc = 'qa_{}.cc'.format(self._info['blockname'])
                cmake.comment_out_lines(fname_qa_cc)
            elif self.info['version'] == '36':
                cmake.comment_out_lines('add_executable.*'+fname)
                cmake.comment_out_lines('target_link_libraries.*'+os.path.splitext(fname)[0])
                cmake.comment_out_lines('GR_ADD_TEST.*'+os.path.splitext(fname)[0])
            self.scm.mark_file_updated(cmake.filename)
            return True
        def _handle_h_swig(cmake, fname):
            """ Comment out include files from the SWIG file,
            as well as the block magic """
            with open(self._file['swig']) as f:
                swigfile = f.read()
            (swigfile, nsubs) = re.subn('(.include\s+"({}/)?{}")'.format(
                                        self.info['modname'], fname),
                                        r'//\1', swigfile)
            if nsubs > 0:
                logger.info("Changing {}...".format(self._file['swig']))
            if nsubs > 1: # Need to find a single BLOCK_MAGIC
                blockname = os.path.splitext(fname[len(self.info['modname'])+1:])[0]
                if self.info['version'] in ('37', '38'):
                    blockname = os.path.splitext(fname)[0]
                (swigfile, nsubs) = re.subn('(GR_SWIG_BLOCK_MAGIC2?.+{}.+;)'.format(blockname), r'//\1', swigfile)
                if nsubs > 1:
                    logger.warning("Hm, changed more then expected while editing {}.".format(self._file['swig']))
            with open(self._file['swig'], 'w') as f:
                f.write(swigfile)
            self.scm.mark_file_updated(self._file['swig'])
            return False
        def _handle_i_swig(cmake, fname):
            """ Comment out include files from the SWIG file,
            as well as the block magic """
            with open(self._file['swig']) as f:
                swigfile = f.read()
            blockname = os.path.splitext(fname[len(self.info['modname'])+1:])[0]
            if self.info['version'] in ('37', '38'):
                blockname = os.path.splitext(fname)[0]
            swigfile = re.sub(r'(%include\s+"'+fname+'")', r'//\1', swigfile)
            logger.info("Changing {}...".format(self._file['swig']))
            swigfile = re.sub('(GR_SWIG_BLOCK_MAGIC2?.+'+blockname+'.+;)', r'//\1', swigfile)
            with open(self._file['swig'], 'w') as f:
                f.write(swigfile)
            self.scm.mark_file_updated(self._file['swig'])
            return False

        # This portion will be covered by the CLI
        if not self.cli:
            self.validate()
        # List of special rules: 0: subdir, 1: filename re match, 2: callback
        special_treatments = (
                ('python', r'qa.+py$', _handle_py_qa),
                ('python', r'^(?!qa).+py$', _handle_py_mod),
                ('lib', r'qa.+\.cc$', _handle_cc_qa),
                ('include/{}'.format(self.info['modname']), r'.+\.h$', _handle_h_swig),
                ('include', r'.+\.h$', _handle_h_swig),
                ('swig', r'.+\.i$', _handle_i_swig)
        )
        for subdir in self._subdirs:
            if self.skip_subdirs[subdir]:
                continue
            if self.info['version'] in ('37', '38') and subdir == 'include':
                subdir = 'include/{}'.format(self.info['modname'])
            try:
                cmake = CMakeFileEditor(os.path.join(subdir, 'CMakeLists.txt'))
            except IOError:
                continue
            logger.info("Traversing {}...".format(subdir))
            filenames = cmake.find_filenames_match(self.info['pattern'])
            yes = self.info['yes']
            for fname in filenames:
                file_disabled = False
                if not yes:
                    ans = cli_input("Really disable {}? [Y/n/a/q]: ".format(fname)).lower().strip()
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
            self.scm.mark_files_updated((os.path.join(subdir, 'CMakeLists.txt'),))
        logger.warning("Careful: 'gr_modtool disable' does not resolve dependencies.")
