#
# Copyright 2013, 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to call bindtool and create Python bindings """


import os
import logging
import warnings
import subprocess

try:
    from gnuradio.bindtool import BindingGenerator
except ImportError:
    have_bindtool = False
else:
    have_bindtool = True

from ..tools import ParserCCBlock, CMakeFileEditor, ask_yes_no, get_block_names
from .base import ModTool, ModToolException

from gnuradio import gr

logger = logging.getLogger(__name__)


class ModToolGenBindings(ModTool):
    """ Make YAML file for GRC block bindings """
    name = 'bind'
    description = 'Generate Python bindings for GR block'

    def __init__(self, blockname=None, **kwargs):
        ModTool.__init__(self, blockname, **kwargs)
        self.info['pattern'] = blockname
        self.info['addl_includes'] = kwargs['addl_includes']
        self.info['define_symbols'] = kwargs['define_symbols']
        self.info['update_hash_only'] = kwargs['update_hash_only']

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
        if not have_bindtool:
            logger.error(
                "Bindtool required to generate bindings  ... Aborting")
            return

        with warnings.catch_warnings():
            warnings.filterwarnings("ignore", category=DeprecationWarning)
            blocknames_to_process = []
            if self.info['blockname']:
                # A complete block name was given
                blocknames_to_process.append(self.info['blockname'])
            elif self.info['pattern']:
                # A regex resembling one or several blocks were given
                blocknames_to_process = get_block_names(
                    self.info['pattern'], self.info['modname'])
            else:
                raise ModToolException("No block name or regex was specified!")

            if self.info['version'] in ['310']:
                prefix_include_root = '/'.join(('gnuradio',
                                               self.info['modname']))
            else:
                prefix_include_root = self.info['modname']

            files_to_process = [os.path.join(
                self.dir, self.info['includedir'], f'{blockname}.h') for blockname in blocknames_to_process]
            bg = BindingGenerator(prefix=gr.prefix(), namespace=[
                'gr', self.info['modname']], prefix_include_root=prefix_include_root, output_dir=self.info['pydir'],
                define_symbols=self.info['define_symbols'], addl_includes=self.info['addl_includes'], update_hash_only=self.info['update_hash_only'])
            for file_to_process in files_to_process:
                if self.info['update_hash_only']:
                    bg.fix_file_hash(file_to_process)
                else:
                    bg.gen_file_binding(file_to_process)
