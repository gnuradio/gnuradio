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
""" Module to generate AST for the headers and parse it """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

import os
import sys
import logging

import click

from pygccxml import parser
from pygccxml import declarations
from pygccxml import utils

from blocktool.core.base import BlockToolException, BlockTool
from blocktool.core.makeyaml import yaml_generator
from blocktool.core.makejson import json_generator

logger = logging.getLogger(__name__)


class BlockToolGenerateAst(BlockTool):
    """
    Enter a header file from GNU Radio module
    """
    name = 'Generate Ast'
    description = 'Create a parsed output from a block header file'
    module_types = []
    target_dir = os.path.join('.', '..', '..')
    for list_dir in os.listdir(target_dir):
        if list_dir.startswith('gr-'):
            list_dir = list_dir.split('-')[-1]
            module_types.append(list_dir)
    module_types.remove('utils')
    module_types = tuple(module_types)

    def __init__(self, module_name=None, file_name=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        self.info['modname'] = module_name
        self.info['filename'] = file_name

    def validate(self):
        """ Validates the arguments """
        BlockTool._validate(self)
        pass

    def ast_generator(self):
        """
        Abstract Syntax Tree text file generator
        magic happens here!
        """
        if self.info['yaml_confirm'] or self.info['json_confirm']:
            generator_path, generator_name = utils.find_xml_generator()
            xml_generator_config = parser.xml_generator_configuration_t(
                xml_generator_path=generator_path,
                xml_generator=generator_name,
                compiler="gcc")
            gr = self.info['modname'].split("-")[0]
            module = self.info['modname'].split("-")[-1]
            decls = parser.parse(
                [self.info['target_file']], xml_generator_config)
            global_namespace = declarations.get_global_namespace(decls)
            ns = global_namespace.namespace(gr)
            main_namespace = ns.namespace(module)
            orig_stdout = sys.stdout
            f = open('temp_ast.txt', 'w')
            sys.stdout = f
            declarations.print_declarations(main_namespace)
            sys.stdout = orig_stdout
            f.close()
        else:
            click.secho("Won't do the hardwork unnecessarily!!", fg='blue')
            exit(1)

    def parse_ast(self):
        """
        Parse the AST generated
        """
        # Create a data dict after parsing the text file
        # pass the data dict to both yaml and json generator
        # do delete the temporary ast file generated
        if self.info['yaml_confirm']:
            yaml_generator(self.info['yaml_confirm'])

        if self.info['json_confirm']:
            json_generator(self.info['json_confirm'])

    def run(self):
        """ Run, run, run. """
        self.validate()
        self.ast_generator()
        self.parse_ast()
