#
# Copyright 2019 Free Software Foundation, Inc.
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
""" Module to generate AST for the headers """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import re
import logging

from pygccxml import parser
from pygccxml import declarations
from pygccxml import utils

from blocktool.core.base import BlockToolException, BlockTool

logger = logging.getLogger(__name__)


class BlockToolGenerateAst(BlockTool):
    """ Select a GNU Radio module. """
    name = 'module'
    description = 'Select a GNU Radio header file to parse it'
    module_types = ()
    target_dir = os.path.join('.', '..', '..')
    for list_dir in os.listdir(target_dir):
        if list_dir.startswith('gr-'):
            module_types+=(list_dir,)

    def __init__(self, modname=None, filename=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)

    def validate(self):
        """ Validates the arguments """
        BlockTool._validate(self)
        pass

    def ast_generator(self):
        """
        Abstract Syntax Tree text file generator
        magic happens here!
        """
        print(str(self.info['modname'])+": "+str(self.info['filename']))
        generator_path, generator_name = utils.find_xml_generator()
        xml_generator_config = parser.xml_generator_configuration_t(
            xml_generator_path=generator_path,
            xml_generator=generator_name,
            compiler="gcc")

        gr = self.info['modname'].split("-")[0]
        module = self.info['modname'].split("-")[-1]
        decls = parser.parse([self.info['filename']], xml_generator_config)
        global_namespace = declarations.get_global_namespace(decls)
        ns = global_namespace.namespace(gr)
        main_namespace = ns.namespace(module)
        return declarations.print_declarations(main_namespace)

    def run(self):
        """ Run, run, run. """
        print(self.info['modname'])
        print(self.info['filename'])
        self.validate()
