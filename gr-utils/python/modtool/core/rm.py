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
""" Remove blocks module """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import sys
import glob
import logging

from ..tools import remove_pattern_from_file, CMakeFileEditor
from ..cli import cli_input
from .base import ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolRemove(ModTool):
    """ Remove block (delete files and remove Makefile entries) """
    name = 'remove'
    description = 'Remove a block from a module.'

    def __init__(self, blockname=None, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['pattern'] = blockname

    def validate(self):
        """ Validates the arguments """
        ModTool._validate(self)
        if not self.info['pattern'] or self.info['pattern'].isspace():
            raise ModToolException("Incorrect blockname (Regex)!")

    def run(self):
        """ Go, go, go! """
        # This portion will be covered by the CLI
        if not self.cli:
            self.validate()
        def _remove_cc_test_case(filename=None, ed=None):
            """ Special function that removes the occurrences of a qa*.cc file
            from the CMakeLists.txt. """
            if filename[:2] != 'qa':
                return
            if self.info['version'] == '37':
                (base, ext) = os.path.splitext(filename)
                if ext == '.h':
                    remove_pattern_from_file(self._file['qalib'],
                                             r'^#include "{}"\s*$'.format(filename))
                    remove_pattern_from_file(self._file['qalib'],
                                             r'^\s*s->addTest\(gr::{}::{}::suite\(\)\);\s*$'.format(
                                                    self.info['modname'], base)
                                            )
                    self.scm.mark_file_updated(self._file['qalib'])
                elif ext == '.cc':
                    ed.remove_value('list',
                                    r'\$\{CMAKE_CURRENT_SOURCE_DIR\}/%s' % filename,
                                    to_ignore_start='APPEND test_{}_sources'.format(self.info['modname']))
                    self.scm.mark_file_updated(ed.filename)
            elif self._info['version'] == '38':
                (base, ext) = os.path.splitext(filename)
                if ext == '.cc':
                    ed.remove_value(
                        'list', filename,
                        to_ignore_start='APPEND test_%s_sources' % self._info['modname'])
                    self.scm.mark_file_updated(ed.filename)
            else:
                filebase = os.path.splitext(filename)[0]
                ed.delete_entry('add_executable', filebase)
                ed.delete_entry('target_link_libraries', filebase)
                ed.delete_entry('GR_ADD_TEST', filebase)
                ed.remove_double_newlines()
                self.scm.mark_file_updated(ed.filename)

        def _remove_py_test_case(filename=None, ed=None):
            """ Special function that removes the occurrences of a qa*.{cc,h} file
            from the CMakeLists.txt and the qa_$modname.cc. """
            if filename[:2] != 'qa':
                return
            filebase = os.path.splitext(filename)[0]
            ed.delete_entry('GR_ADD_TEST', filebase)
            ed.remove_double_newlines()

        def _make_swig_regex(filename):
            filebase = os.path.splitext(filename)[0]
            pyblockname = filebase.replace(self.info['modname'] + '_', '')
            regexp = r'(^\s*GR_SWIG_BLOCK_MAGIC2?\({},\s*{}\);|^\s*.include\s*"({}/)?{}"\s*)'.format \
                    (self.info['modname'], pyblockname, self.info['modname'], filename)
            return regexp
        # Go, go, go!
        if not self.skip_subdirs['lib']:
            self._run_subdir('lib', ('*.cc', '*.h'), ('add_library', 'list'),
                             cmakeedit_func=_remove_cc_test_case)
        if not self.skip_subdirs['include']:
            incl_files_deleted = self._run_subdir(self.info['includedir'], ('*.h',), ('install',))
        if not self.skip_subdirs['swig']:
            swig_files_deleted = self._run_subdir('swig', ('*.i',), ('install',))
            for f in incl_files_deleted + swig_files_deleted:
                # TODO do this on all *.i files
                remove_pattern_from_file(self._file['swig'], _make_swig_regex(f))
                self.scm.mark_file_updated(self._file['swig'])
        if not self.skip_subdirs['python']:
            py_files_deleted = self._run_subdir('python', ('*.py',), ('GR_PYTHON_INSTALL',),
                                                cmakeedit_func=_remove_py_test_case)
            for f in py_files_deleted:
                remove_pattern_from_file(self._file['pyinit'], r'.*import\s+{}.*'.format(f[:-3]))
                remove_pattern_from_file(self._file['pyinit'], r'.*from\s+{}\s+import.*\n'.format(f[:-3]))
        if not self.skip_subdirs['grc']:
            self._run_subdir('grc', ('*.yml',), ('install',))

    def _run_subdir(self, path, globs, makefile_vars, cmakeedit_func=None):
        """ Delete all files that match a certain pattern in path.
        path - The directory in which this will take place
        globs - A tuple of standard UNIX globs of files to delete (e.g. *.yml)
        makefile_vars - A tuple with a list of CMakeLists.txt-variables which
                        may contain references to the globbed files
        cmakeedit_func - If the CMakeLists.txt needs special editing, use this
        """
        # 1. Create a filtered list
        files = []
        for g in globs:
            files = files + sorted(glob.glob("{}/{}".format(path, g)))
        files_filt = []
        logger.info("Searching for matching files in {}/:".format(path))
        for f in files:
            if re.search(self.info['pattern'], os.path.basename(f)) is not None:
                files_filt.append(f)
        if len(files_filt) == 0:
            logger.info("None found.")
            return []
        # 2. Delete files, Makefile entries and other occurrences
        files_deleted = []
        ed = CMakeFileEditor('{}/CMakeLists.txt'.format(path))
        yes = self.info['yes']
        for f in files_filt:
            b = os.path.basename(f)
            if not yes:
                ans = cli_input("Really delete {}? [Y/n/a/q]: ".format(f)).lower().strip()
                if ans == 'a':
                    yes = True
                if ans == 'q':
                    sys.exit(0)
                if ans == 'n':
                    continue
            files_deleted.append(b)
            logger.info("Deleting {}.".format(f))
            self.scm.remove_file(f)
            os.unlink(f)
            logger.info("Deleting occurrences of {} from {}/CMakeLists.txt...".format(b, path))
            for var in makefile_vars:
                ed.remove_value(var, b)
            if cmakeedit_func is not None:
                cmakeedit_func(b, ed)
        ed.write()
        self.scm.mark_files_updated(('{}/CMakeLists.txt'.format(path)))
        return files_deleted
