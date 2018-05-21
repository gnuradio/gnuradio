#
# Copyright 2014 Free Software Foundation, Inc.
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
""" Module to rename blocks """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import glob

from .util_functions import append_re_line_sequence, ask_yes_no
from .util_functions import SequenceCompleter
from .cmakefile_editor import CMakeFileEditor
from .modtool_base import ModTool, ModToolException
from .templates import Templates

def get_block_candidates():
    cpp_filters = ["*.cc", "*.cpp"]
    cpp_blocks = []
    for ftr in cpp_filters:
        cpp_blocks += filter(lambda x: not (x.startswith('qa_') or
                             x.startswith('test_')),
                             glob.glob1("lib", ftr))
    python_blocks = filter(lambda x: not (x.startswith('qa_') or
                           x.startswith('build') or
                           x.startswith('__init__')),
                           glob.glob1("python", "*.py"))
    block_candidates = [x.split('_impl')[0] for x in cpp_blocks] + [x.split('.')[0] for x in python_blocks]
    return block_candidates


class ModToolRename(ModTool):
    """ Rename a block in the out-of-tree module. """
    name = 'rename'
    description = 'Rename block inside module.'

    def __init__(self):
        ModTool.__init__(self)
        self._add_cc_qa = False
        self._add_py_qa = False
        self._skip_cmakefiles = False
        self._license_file = None

    @staticmethod
    def setup_parser(parser):
         #parser = parser.add_argument_group(title="Rename module options")
        ModTool.setup_parser_block(parser)
        parser.add_argument("new_name", nargs="?", metavar='NEW-BLOCK-NAME',
                help="New name of the block.")
        return parser

    def setup(self, options):
        ModTool.setup(self, options)

        if ((self._skip_subdirs['lib'] and self._info['lang'] == 'cpp')
                or (self._skip_subdirs['python'] and self._info['lang'] == 'python')):
            raise ModToolException('Missing or skipping relevant subdir.')

        # first make sure the old block name is provided
        self._info['oldname'] = options.blockname
        if self._info['oldname'] is None:
            self._info['oldname'] = input("Enter name of block/code to rename (without module name prefix): ")
        else:
            block_candidates = get_block_candidates()
            with SequenceCompleter(block_candidates):
                self._info['oldname'] = \
                    raw_input("Enter name of block/code to rename (without module name prefix): ")
        if not re.match('[a-zA-Z0-9_]+', self._info['oldname']):
            raise ModToolException('Invalid block name.')
        print("Block/code to rename identifier: " + self._info['oldname'])
        self._info['fulloldname'] = self._info['modname'] + '_' + self._info['oldname']

        # now get the new block name
        if options.new_name is None:
            self._info['newname'] = input("Enter name of block/code (without module name prefix): ")
        else:
            self._info['newname'] = options.new_name[0]
        if not re.match('[a-zA-Z0-9_]+', self._info['newname']):
            raise ModToolException('Invalid block name.')
        print("Block/code identifier: " + self._info['newname'])
        self._info['fullnewname'] = self._info['modname'] + '_' + self._info['newname']

    def run(self, options):
        """ Go, go, go. """
        self.setup(options)
        module = self._info['modname']
        oldname = self._info['oldname']
        newname = self._info['newname']
        print("In module '%s' rename block '%s' to '%s'" % (module, oldname, newname))
        self._run_swig_rename(self._file['swig'], oldname, newname)
        self._run_grc_rename(self._info['modname'], oldname, newname)
        self._run_python_qa(self._info['modname'], oldname, newname)
        self._run_python(self._info['modname'], oldname, newname)
        self._run_lib(self._info['modname'], oldname, newname)
        self._run_include(self._info['modname'], oldname, newname)
        return

    def _run_swig_rename(self, swigfilename, old, new):
        """ Rename SWIG includes and block_magic """
        nsubs = self._run_file_replace(swigfilename, old, new)
        if nsubs < 1:
            print("Couldn't find '%s' in file '%s'." % (old, swigfilename))
        if nsubs == 2:
            print("Changing 'noblock' type file")
        if nsubs > 3:
            print("Hm, changed more then expected while editing %s." % swigfilename)
        return False

    def _run_lib(self, module, old, new):
        ccfile = './lib/' + old + '_impl.cc'
        if not os.path.isfile(ccfile):  # in case it is a 'noblock'
            ccfile = './lib/' + old + '.cc'
        hfile = './lib/' + old + '_impl.h'
        self._run_file_replace(ccfile, old, new)
        self._run_file_replace(hfile, old, new)
        self._run_file_replace(hfile, old.upper(), new.upper())  # take care of include guards
        self._run_cmakelists('./lib/', old, new)
        self._run_file_rename('./lib/', old, new)
        self._run_cpp_qa(module, old, new)

    def _run_cpp_qa(self, module, old, new):
        path = './lib/'
        filename = 'qa_' + module + '.cc'
        nsubs = self._run_file_replace(path + filename, old, new)
        if nsubs > 0:
            print("C++ QA code detected, renaming...")
            filename = 'qa_' + old + '.cc'
            self._run_file_replace(path + filename, old, new)
            filename = 'qa_' + old + '.h'
            self._run_file_replace(path + filename, old, new)
            self._run_file_replace(path + filename, old.upper(), new.upper())
            self._run_file_rename(path, 'qa_' + old, 'qa_' + new)
        else:
            print("No C++ QA code detected, skipping...")

    def _run_include(self, module, old, new):
        path = './include/' + module + '/'
        filename = path + old + '.h'
        self._run_file_replace(filename, old, new)
        self._run_file_replace(filename, old.upper(), new.upper())  # take care of include guards
        self._run_cmakelists(path, old, new)
        self._run_file_rename(path, old, new)

    def _run_python(self, module, old, new):
        path = './python/'
        filename = '__init__.py'
        nsubs = self._run_file_replace(path + filename, old, new)
        if nsubs > 0:
            print("Python block detected, renaming...")
            filename = old + '.py'
            self._run_file_replace(path + filename, old, new)
            self._run_cmakelists(path, old, new)
            self._run_file_rename(path, old, new)
        else:
            print("Not a Python block, nothing to do here...")

    def _run_python_qa(self, module, old, new):
        new = 'qa_' + new
        old = 'qa_' + old
        filename = './python/' + old + '.py'
        self._run_file_replace(filename, old, new)
        self._run_cmakelists('./python/', old, new)
        self._run_file_rename('./python/', old, new)

    def _run_grc_rename(self, module, old, new):
        grcfile = './grc/' + module + '_' + old + '.xml'
        self._run_file_replace(grcfile, old, new)
        self._run_cmakelists('./grc/', old, new)
        self._run_file_rename('./grc/', module + '_' + old, module + '_' + new)

    def _run_cmakelists(self, path, first, second):
        filename = path + 'CMakeLists.txt'
        nsubs = self._run_file_replace(filename, first, second)
        if nsubs < 1:
            print("'%s' wasn't in '%s'." % (first, filename))

    def _run_file_rename(self, path, old, new):
        files = os.listdir(path)
        for file in files:
            if file.find(old) > -1 and file.find(old) < 3:
                nl = file.replace(old, new)
                src = path + file
                dst = path + nl
                print("Renaming file '%s' to '%s'." % (src, dst))
                os.rename(src, dst)

    def _run_file_replace(self, filename, old, new):
        if not os.path.isfile(filename):
            return False
        else:
            print("In '%s' renaming occurrences of '%s' to '%s'" % (filename, old, new))

        cfile = open(filename).read()
        (cfile, nsubs) = re.subn(old, new, cfile)

        open(filename, 'w').write(cfile)
        self.scm.mark_file_updated(filename)
        return nsubs
