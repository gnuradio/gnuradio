#
# Copyright 2013, 2018, 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to call bindtool and create Python bindings """


from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

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

from ..tools import ParserCCBlock, CMakeFileEditor, ask_yes_no
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
            file_to_process = os.path.join(self.dir, self.info['includedir'], self.info['blockname'] + '.h')
            bg = BindingGenerator(prefix=gr.prefix(), namespace=[
                                  'gr', self.info['modname']], prefix_include_root=self.info['modname'], output_dir=os.path.join(self.dir, 'python'))
            bg.gen_file_binding(file_to_process)
