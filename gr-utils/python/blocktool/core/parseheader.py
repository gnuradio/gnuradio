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
    name = 'Parse Header'
    description = 'Create a parsed output from a block header file'
    module_types = []
    target_dir = os.path.join('.', '..', '..')
    for list_dir in os.listdir(target_dir):
        if list_dir.startswith('gr-'):
            list_dir = list_dir.split('-')[-1]
            module_types.append(list_dir)
    module_types.remove('utils')
    module_types = tuple(module_types)

    def __init__(self, module_name=None, file_name=None,
                 cli_confirm=False, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        self.info['modname'] = module_name
        self.info['filename'] = file_name
        self.info['cli'] = cli_confirm

    def validate(self):
        """ Override the Blocktool validate function """
        BlockTool._validate(self)
        if not self.info['cli']:
            self._validate()
            # Verify the arguments when called as an API
            if self.info['modname'] is None:
                raise BlockToolException('module_name cannot be None')
            if self.info['filename'] is None:
                raise BlockToolException('file_name name cannot be None')
            if self.info['modname'].startswith('gr-'):
                _module = self.info['modname'].split('-')[-1]
            else:
                _module = self.info['modname']
                self.info['modname'] = 'gr-'+_module
            if _module not in self.module_types:
                raise BlockToolException('No module with name ' +
                                         self.info['modname']+' found')
            _extension = self.info['filename'].split('.')[-1]
            if _extension is not 'h':
                raise BlockToolException(
                    'header files have extension .h only!')
            self.info['target_dir'] = os.path.join('.', '..', '..',
                                                   self.info['modname'],
                                                   'include',
                                                   'gnuradio',
                                                   self.info['modname'].split('-')[-1])
            _header_list = os.listdir(self.info['target_dir'])
            if self.info['filename'] not in _header_list:
                raise BlockToolException('Please enter a header file from '
                                         + self.info['modname'] +
                                         ' module only!')
            self.info['target_file'] = os.path.join(self.info['target_dir'],
                                                    self.info['filename'])

    def get_header_info(self):
        """
        Abstract Syntax Tree text file generator
        magic happens here!
        : Parse the AST generated
        : Can be used as an CLI command or an extenal API
        """
        # Create a data dict after parsing the text file
        # pass the data dict to both yaml and json generator
        # do delete the temporary ast file generated

        if not self.info['cli']:
            self.validate()
        else:
            if not self.info['yaml_confirm'] and not self.info['json_confirm']:
                click.secho("Won't do the hardwork unnecessarily!!", fg='blue')
                exit(1)

        gr = self.info['modname'].split("-")[0]
        module = self.info['modname'].split("-")[-1]
        generator_path, generator_name = utils.find_xml_generator()
        xml_generator_config = parser.xml_generator_configuration_t(
            xml_generator_path=generator_path,
            xml_generator=generator_name,
            compiler="gcc")
        decls = parser.parse(
            [self.info['target_file']], xml_generator_config)
        global_namespace = declarations.get_global_namespace(decls)
        ns = global_namespace.namespace(gr)
        main_namespace = ns.namespace(module)
        orig_stdout = sys.stdout
        file_path = os.path.join('.', 'temp_ast.txt')
        f = open(file_path, 'w')
        sys.stdout = f
        declarations.print_declarations(main_namespace)
        sys.stdout = orig_stdout
        f.close()

        if self.info['cli']:
            if self.info['yaml_confirm']:
                yaml_generator(self.info['yaml_confirm'])

            if self.info['json_confirm']:
                json_generator(self.info['json_confirm'])

        parse_data = "sample"
        return parse_data

    def run(self):
        """ Run, run, run. """
        self.validate()
        self.get_header_info()
