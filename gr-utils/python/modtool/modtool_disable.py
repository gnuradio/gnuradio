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
""" Disable blocks module """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import sys

from .modtool_base import ModTool
from .cmakefile_editor import CMakeFileEditor


class ModToolDisable(ModTool):
    """ Disable block (comments out CMake entries for files) """
    name = 'disable'
    description = 'Disable selected block in module.'

    def __init__(self):
        ModTool.__init__(self)

    @staticmethod
    def setup_parser(parser):
        ModTool.setup_parser_block(parser)

    def setup(self, options):
        ModTool.setup(self, options)

        if options.blockname is not None:
            self._info['pattern'] = options.blockname
        else:
            self._info['pattern'] = input('Which blocks do you want to disable? (Regex): ')
        if len(self._info['pattern']) == 0:
            self._info['pattern'] = '.'

    def run(self, options):
        """ Go, go, go! """
        def _handle_py_qa(cmake, fname):
            """ Do stuff for py qa """
            cmake.comment_out_lines('GR_ADD_TEST.*'+fname)
            self.scm.mark_file_updated(cmake.filename)
            return True
        def _handle_py_mod(cmake, fname):
            """ Do stuff for py extra files """
            try:
                initfile = open(self._file['pyinit']).read()
            except IOError:
                print("Could not edit __init__.py, that might be a problem.")
                return False
            pymodname = os.path.splitext(fname)[0]
            initfile = re.sub(r'((from|import)\s+\b'+pymodname+r'\b)', r'#\1', initfile)
            open(self._file['pyinit'], 'w').write(initfile)
            self.scm.mark_file_updated(self._file['pyinit'])
            return False
        def _handle_cc_qa(cmake, fname):
            """ Do stuff for cc qa """
            if self._info['version'] == '37':
                cmake.comment_out_lines(r'\$\{CMAKE_CURRENT_SOURCE_DIR\}/'+fname)
                fname_base = os.path.splitext(fname)[0]
                ed = CMakeFileEditor(self._file['qalib']) # Abusing the CMakeFileEditor...
                ed.comment_out_lines(r'#include\s+"%s.h"' % fname_base, comment_str='//')
                ed.comment_out_lines(r'%s::suite\(\)' % fname_base, comment_str='//')
                ed.write()
                self.scm.mark_file_updated(self._file['qalib'])
            elif self._info['version'] == '38':
                fname_qa_cc = 'qa_{}.cc'.format(self._info['blockname'])
                cmake.comment_out_lines(fname_qa_cc)
            elif self._info['version'] == '36':
                cmake.comment_out_lines('add_executable.*'+fname)
                cmake.comment_out_lines('target_link_libraries.*'+os.path.splitext(fname)[0])
                cmake.comment_out_lines('GR_ADD_TEST.*'+os.path.splitext(fname)[0])
            self.scm.mark_file_updated(cmake.filename)
            return True
        def _handle_h_swig(cmake, fname):
            """ Comment out include files from the SWIG file,
            as well as the block magic """
            swigfile = open(self._file['swig']).read()
            (swigfile, nsubs) = re.subn(r'(.include\s+"(%s/)?%s")' % (
                self._info['modname'], fname), r'//\1', swigfile)
            if nsubs > 0:
                print("Changing %s..." % self._file['swig'])
            if nsubs > 1: # Need to find a single BLOCK_MAGIC
                blockname = os.path.splitext(fname[len(self._info['modname'])+1:])[0]
                if self._info['version'] in ('37', '38'):
                    blockname = os.path.splitext(fname)[0]
                (swigfile, nsubs) = re.subn('(GR_SWIG_BLOCK_MAGIC2?.+%s.+;)' % blockname, r'//\1', swigfile)
                if nsubs > 1:
                    print("Hm, changed more then expected while editing %s." % self._file['swig'])
            open(self._file['swig'], 'w').write(swigfile)
            self.scm.mark_file_updated(self._file['swig'])
            return False
        def _handle_i_swig(cmake, fname):
            """ Comment out include files from the SWIG file,
            as well as the block magic """
            swigfile = open(self._file['swig']).read()
            blockname = os.path.splitext(fname[len(self._info['modname'])+1:])[0]
            if self._info['version'] in ('37', '38'):
                blockname = os.path.splitext(fname)[0]
            swigfile = re.sub(r'(%include\s+"'+fname+r'")', r'//\1', swigfile)
            print("Changing %s..." % self._file['swig'])
            swigfile = re.sub('(GR_SWIG_BLOCK_MAGIC2?.+'+blockname+'.+;)', r'//\1', swigfile)
            open(self._file['swig'], 'w').write(swigfile)
            self.scm.mark_file_updated(self._file['swig'])
            return False
        self.setup(options)
        # List of special rules: 0: subdir, 1: filename re match, 2: callback
        special_treatments = (
            ('python', r'qa.+py$', _handle_py_qa),
            ('python', r'^(?!qa).+py$', _handle_py_mod),
            ('lib', r'qa.+\.cc$', _handle_cc_qa),
            ('include/%s' % self._info['modname'], r'.+\.h$', _handle_h_swig),
            ('include', r'.+\.h$', _handle_h_swig),
            ('swig', r'.+\.i$', _handle_i_swig)
        )
        for subdir in self._subdirs:
            if self._skip_subdirs[subdir]:
                continue
            if self._info['version'] in ('37', '38') and subdir == 'include':
                subdir = 'include/%s' % self._info['modname']
            try:
                cmake = CMakeFileEditor(os.path.join(subdir, 'CMakeLists.txt'))
            except IOError:
                continue
            print("Traversing %s..." % subdir)
            filenames = cmake.find_filenames_match(self._info['pattern'])
            yes = self._info['yes']
            for fname in filenames:
                file_disabled = False
                if not yes:
                    ans = input("Really disable %s? [Y/n/a/q]: " % fname).lower().strip()
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
        print("Careful: 'gr_modtool disable' does not resolve dependencies.")
