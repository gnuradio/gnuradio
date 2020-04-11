#
# Copyright 2014, 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to rename blocks """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import logging

from .base import get_block_candidates, ModTool, ModToolException

logger = logging.getLogger(__name__)


class ModToolRename(ModTool):
    """ Rename a block in the out-of-tree module. """
    name = 'rename'
    description = 'Rename a block inside a module.'

    def __init__(self, blockname=None, new_name=None, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['oldname'] = blockname
        self.info['newname'] = new_name

    def validate(self):
        """ Validates the arguments """
        ModTool._validate(self)
        if not self.info['oldname']:
            raise ModToolException('Old block name (blockname) not specified.')
        if not re.match('[a-zA-Z0-9_]+', self.info['oldname']):
            raise ModToolException('Invalid block name.')
        block_candidates = get_block_candidates()
        if self.info['oldname'] not in block_candidates:
            choices = [x for x in block_candidates if self.info['oldname'] in x]
            if len(choices)>0:
                print("Suggested alternatives: "+str(choices))
            raise ModToolException("Blockname for renaming does not exists!")
        if not self.info['newname']:
            raise ModToolException('New blockname (new_name) not specified.')
        if not re.match('[a-zA-Z0-9_]+', self.info['newname']):
            raise ModToolException('Invalid new block name.')

    def assign(self):
        self.info['fullnewname'] = self.info['modname'] + '_' + self.info['newname']

    def run(self):
        """ Go, go, go. """
        # This portion will be covered by the CLI
        if not self.cli:
            self.validate()
            self.assign()
        module = self.info['modname']
        oldname = self.info['oldname']
        newname = self.info['newname']
        logger.info(f"In module '{module}' rename block '{oldname}' to '{newname}'")
        self._run_swig_rename(self._file['swig'], oldname, newname)
        self._run_grc_rename(self.info['modname'], oldname, newname)
        self._run_python_qa(self.info['modname'], oldname, newname)
        self._run_python(self.info['modname'], oldname, newname)
        self._run_lib(self.info['modname'], oldname, newname)
        self._run_include(self.info['modname'], oldname, newname)
        return

    def _run_swig_rename(self, swigfilename, old, new):
        """ Rename SWIG includes and block_magic """
        nsubs = self._run_file_replace(swigfilename, old, new)
        if nsubs < 1:
            logger.info(f"Couldn't find '{old}' in file '{swigfilename}'.")
        if nsubs == 2:
            logger.info("Changing 'noblock' type file")
        if nsubs > 3:
            logger.warning(f"Hm, changed more then expected while editing {swigfilename}.")
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
            logger.info("C++ QA code detected, renaming...")
            filename = 'qa_' + old + '.cc'
            self._run_file_replace(path + filename, old, new)
            filename = 'qa_' + old + '.h'
            self._run_file_replace(path + filename, old, new)
            self._run_file_replace(path + filename, old.upper(), new.upper())
            self._run_file_rename(path, 'qa_' + old, 'qa_' + new)
        else:
            logger.info("No C++ QA code detected, skipping...")

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
            logger.info("Python block detected, renaming...")
            filename = old + '.py'
            self._run_file_replace(path + filename, old, new)
            self._run_cmakelists(path, old, new)
            self._run_file_rename(path, old, new)
        else:
            logger.info("Not a Python block, nothing to do here...")

    def _run_python_qa(self, module, old, new):
        new = 'qa_' + new
        old = 'qa_' + old
        filename = './python/' + old + '.py'
        self._run_file_replace(filename, old, new)
        self._run_cmakelists('./python/', old, new)
        self._run_file_rename('./python/', old, new)

    def _run_grc_rename(self, module, old, new):
        grcfile = './grc/' + module + '_' + old + '.yml'
        self._run_file_replace(grcfile, old, new)
        self._run_cmakelists('./grc/', old, new)
        self._run_file_rename('./grc/', module + '_' + old, module + '_' + new)

    def _run_cmakelists(self, path, first, second):
        filename = path + 'CMakeLists.txt'
        nsubs = self._run_file_replace(filename, first, second)
        if nsubs < 1:
            logger.info(f"'{first}' wasn't in '{filename}'.")

    def _run_file_rename(self, path, old, new):
        files = os.listdir(path)
        for file in files:
            if file.find(old) > -1 and file.find(old) < 3:
                nl = file.replace(old, new)
                src = path + file
                dst = path + nl
                logger.info(f"Renaming file '{src}' to '{dst}'.")
                os.rename(src, dst)

    def _run_file_replace(self, filename, old, new):
        if not os.path.isfile(filename):
            return False
        else:
            logger.info(f"In '{filename}' renaming occurrences of '{old}' to '{new}'")

        with open(filename) as f:
            cfile = f.read()
        (cfile, nsubs) = re.subn(old, new, cfile)

        with open(filename, 'w') as f:
            f.write(cfile)
        self.scm.mark_file_updated(filename)
        return nsubs
