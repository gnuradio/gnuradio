#
# Copyright 2013, 2018-2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Remove blocks module """


import os
import re
import sys
import glob
import logging

from ..tools import remove_pattern_from_file, CMakeFileEditor, CPPFileEditor, get_block_names
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
        else:
            from ..cli import cli_input

        def _remove_cc_test_case(filename=None, ed=None):
            """ Special function that removes the occurrences of a qa*.cc file
            from the CMakeLists.txt. """
            modname_ = self.info['modname']
            if filename[:2] != 'qa':
                return
            if self.info['version'] == '37':
                (base, ext) = os.path.splitext(filename)
                if ext == '.h':
                    remove_pattern_from_file(self._file['qalib'],
                                             fr'^#include "{filename}"\s*$')
                    remove_pattern_from_file(self._file['qalib'],
                                             fr'^\s*s->addTest\(gr::{modname_}::{base}::suite\(\)\);\s*$'
                                             )
                    self.scm.mark_file_updated(self._file['qalib'])
                elif ext == '.cc':
                    ed.remove_value('list',
                                    r'\$\{CMAKE_CURRENT_SOURCE_DIR\}/%s' % filename,
                                    to_ignore_start=f'APPEND test_{modname_}_sources')
                    self.scm.mark_file_updated(ed.filename)
            elif self.info['version'] in ['38', '310']:
                (base, ext) = os.path.splitext(filename)
                if ext == '.cc':
                    ed.remove_value(
                        'list', filename,
                        to_ignore_start=f'APPEND test_{modname_}_sources')
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

        # Go, go, go!
        if not self.skip_subdirs['python']:
            py_files_deleted = self._run_subdir(self.info['pydir'], ('*.py',), ('GR_PYTHON_INSTALL',),
                                                cmakeedit_func=_remove_py_test_case)
            for f in py_files_deleted:
                remove_pattern_from_file(
                    self._file['pyinit'], fr'.*import\s+{f[:-3]}.*')
                remove_pattern_from_file(
                    self._file['pyinit'], fr'.*from\s+{f[:-3]}\s+import.*\n')

            pb_files_deleted = self._run_subdir(os.path.join(
                self.info['pydir'], 'bindings'), ('*.cc',), ('list',))

            pbdoc_files_deleted = self._run_subdir(os.path.join(
                self.info['pydir'], 'bindings', 'docstrings'), ('*.h',), ('',))

            # Update python_bindings.cc
            blocknames_to_delete = []
            if self.info['blockname']:
                # A complete block name was given
                blocknames_to_delete.append(self.info['blockname'])
            elif self.info['pattern']:
                # A regex resembling one or several blocks were given
                blocknames_to_delete = get_block_names(
                    self.info['pattern'], self.info['modname'])
            else:
                raise ModToolException("No block name or regex was specified!")
            for blockname in blocknames_to_delete:
                ed = CPPFileEditor(self._file['ccpybind'])
                ed.remove_value('// BINDING_FUNCTION_PROTOTYPES(', '// ) END BINDING_FUNCTION_PROTOTYPES',
                                'void bind_' + blockname + '(py::module& m);')
                ed.remove_value('// BINDING_FUNCTION_CALLS(', '// ) END BINDING_FUNCTION_CALLS',
                                'bind_' + blockname + '(m);')
                ed.write()

        if not self.skip_subdirs['lib']:
            self._run_subdir('lib', ('*.cc', '*.h'), ('add_library', 'list'),
                             cmakeedit_func=_remove_cc_test_case)
        if not self.skip_subdirs['include']:
            incl_files_deleted = self._run_subdir(
                self.info['includedir'], ('*.h',), ('install',))
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
        if self.cli:
            from ..cli import cli_input
        # 1. Create a filtered list
        files = []
        for g in globs:
            files = files + sorted(glob.glob(f"{path}/{g}"))
        files_filt = []
        logger.info(f"Searching for matching files in {path}/:")
        if self.info['blockname']:
            # Ensure the blockname given is not confused with similarly named blocks
            blockname_pattern = ''
            if path == self.info['pydir']:
                blockname_pattern = f"^(qa_)?{self.info['blockname']}.py$"
            elif path == os.path.join(self.info['pydir'], 'bindings'):
                blockname_pattern = f"^{self.info['blockname']}_python.cc$"
            elif path == os.path.join(self.info['pydir'], 'bindings', 'docstrings'):
                blockname_pattern = f"^{self.info['blockname']}_pydoc_template.h$"
            elif path == 'lib':
                blockname_pattern = f"^{self.info['blockname']}_impl(\\.h|\\.cc)$"
            elif path == self.info['includedir']:
                blockname_pattern = f"^{self.info['blockname']}.h$"
            elif path == 'grc':
                blockname_pattern = f"^{self.info['modname']}_{self.info['blockname']}.block.yml$"
            for f in files:
                if re.search(blockname_pattern, os.path.basename(f)) is not None:
                    files_filt.append(f)
        elif self.info['pattern']:
            # A regex resembling one or several blocks were given as stdin
            for f in files:
                if re.search(self.info['pattern'], os.path.basename(f)) is not None:
                    files_filt.append(f)
        if len(files_filt) == 0:
            logger.info("None found.")
            return []
        # 2. Delete files, Makefile entries and other occurrences
        files_deleted = []
        yes = self.info['yes']
        for f in files_filt:
            b = os.path.basename(f)
            if not yes and self.cli:
                ans = cli_input(
                    f"Really delete {f}? [Y/n/a/q]: ").lower().strip()
                if ans == 'a':
                    yes = True
                if ans == 'q':
                    sys.exit(0)
                if ans == 'n':
                    continue
            files_deleted.append(b)
            logger.info(f"Deleting {f}.")
            self.scm.remove_file(f)
            os.unlink(f)

            if (os.path.exists(f'{path}/CMakeLists.txt')):
                ed = CMakeFileEditor(f'{path}/CMakeLists.txt')
                logger.info(
                    f"Deleting occurrences of {b} from {path}/CMakeLists.txt...")
                for var in makefile_vars:
                    ed.remove_value(var, b)
                if cmakeedit_func is not None:
                    cmakeedit_func(b, ed)
                ed.write()
                self.scm.mark_files_updated((f'{path}/CMakeLists.txt'))

        return files_deleted
