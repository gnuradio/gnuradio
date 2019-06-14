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
import re
import codecs
import logging

import click

from pygccxml import parser
from pygccxml import declarations
from pygccxml import utils

from blocktool.core.base import BlockToolException, BlockTool
from blocktool.core.makeyaml import yaml_generator
from blocktool.core.makejson import json_generator

LOGGER = logging.getLogger(__name__)


class BlockHeaderParser(BlockTool):
    """
    : Single argument required: file_path
    file_path: enter path for the header block in any of GNU Radio module
    : cli_confirm to remain False if called as an API
        : returns the parsed header data in python dict
        : return dict keys: namespace, class, io_signature, make,
                       properties, methods
    : Can be used as an CLI command or an extenal API
    """
    name = 'Parse Header'
    description = 'Create a parsed output from a block header file'
    module_types = []
    header_list = []
    current_folder = os.path.abspath(os.path.dirname(__file__))
    target_dir = os.path.abspath(os.path.join(current_folder,
                                              '..', '..', '..', '..'))
    for list_dir in os.listdir(target_dir):
        if list_dir.startswith('gr-'):
            list_dir = list_dir.split('-')[-1]
            module_types.append(list_dir)
    module_types.remove('utils')
    module_types = tuple(module_types)
    parsed_data = {}

    def __init__(self, cli_confirm=False, file_path=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        self.info['cli'] = cli_confirm
        if not self.info['cli']:
            if file_path is None:
                raise BlockToolException(
                    'please specify the file path of the header!')
            else:
                file_path = os.path.abspath(file_path)
                try:
                    open(file_path, 'r')
                except OSError as e:
                    raise e
                self.info['target_file'] = file_path
                self.info['modname'] = os.path.basename(os.path.dirname(
                    os.path.dirname(os.path.dirname(os.path.dirname(file_path)))))
                self.info['filename'] = os.path.basename(file_path)
                self.info['target_dir'] = os.path.dirname(file_path)

    def validate(self):
        """ Override the Blocktool validate function """
        BlockTool._validate(self)
        if not self.info['cli']:
            self._validate()
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
                raise BlockToolException("Module must be one of {}.".format(
                    ', '.join(self.module_types)))
            if not self.info['filename'].endswith('.h'):
                raise BlockToolException(
                    'header files have extension .h only!')
            _target_dir = os.path.join(self.target_dir,
                                       self.info['modname'],
                                       'include',
                                       'gnuradio',
                                       self.info['modname'].split('-')[-1])
            if _target_dir != self.info['target_dir']:
                raise Exception("public header not in correct directory")
            for header in os.listdir(self.info['target_dir']):
                if header.endswith('.h'):
                    self.header_list.append(header)
            if self.info['filename'] not in self.header_list:
                raise BlockToolException('Please enter a header file from '
                                         + self.info['modname'] +
                                         ' module only!')

    def get_header_info(self):
        """
        PyGCCXML header code parser
        magic happens here!
        : returns the parsed header data in python dict
        : return dict keys: namespace, class, io_signature, make,
                       properties, methods
        : Can be used as an CLI command or an extenal API
        """
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

        # namespace
        try:
            self.parsed_data['namespace'] = []
            ns = global_namespace.namespace(gr)
            if ns is None:
                raise Exception
            else:
                main_namespace = ns.namespace(module)
            if main_namespace is None:
                raise Exception
            else:
                self.parsed_data['namespace'] = [gr, module]
        except Exception as e:
            raise Exception("Must be a header with block api!\n"+e)

        # class
        try:
            self.parsed_data['class'] = ""
            for _class in main_namespace.declarations:
                if isinstance(_class, declarations.class_t):
                    main_class = _class
                    self.parsed_data['class'] = str(_class).split("::")[
                        2].split(" ")[0]
        except:
            pass

        # make
        try:
            self.parsed_data['make'] = {}
            self.parsed_data['make']['arguments'] = []
            # criteria = declarations.calldef_matcher(name="make")
            # make_f = declarations.matcher.get_single(criteria, main_class)
            # print(make_f)
            # need to work on default values
            # default_values = re.findall(r"[-+]?\d*\.\d+|\d+", str(make_f).split("make")[-1])
            # print(default_values, ", default")
            query_m = declarations.custom_matcher_t(
                lambda mem_fun: mem_fun.name.startswith('make'))
            query_make = query_m & declarations.access_type_matcher_t('public')
            make_func = main_class.member_functions(function=query_make,
                                                    allow_empty=True,
                                                    header_file=self.info['target_file'])
            if make_func:
                for make in make_func:
                    # print((make.return_type))
                    for arg in make.arguments:
                        make_arguments = {
                            "name": str(arg.name),
                            "dtype": str(arg.decl_type),
                            "default": ""
                        }
                        self.parsed_data['make']['arguments'].append(
                            make_arguments.copy())
        except:
            self.parsed_data['make'] = {}
            self.parsed_data['make']['arguments'] = []

        # setters
        try:
            self.parsed_data['methods'] = []
            query_methods = declarations.access_type_matcher_t('public')
            setters = main_class.member_functions(function=query_methods,
                                                  return_type="void",
                                                  allow_empty=True,
                                                  header_file=self.info['target_file'])
            getter_arguments = []
            if setters:
                for setter in setters:
                    setter_args = {
                        "name": str(setter.name),
                        "arguments type": []
                    }
                    for argument in setter.arguments:
                        args = {
                            "name": str(argument.name),
                            "dtype": str(argument.decl_type)
                        }
                        getter_arguments.append(args["name"])
                        setter_args['arguments type'].append(args.copy())
                    self.parsed_data['methods'].append(setter_args.copy())
        except:
            self.parsed_data['methods'] = []

        # getters
        try:
            self.parsed_data['properties'] = []
            query_properties = declarations.access_type_matcher_t('public')
            getters = main_class.member_functions(function=query_properties,
                                                  allow_empty=True,
                                                  header_file=self.info['target_file'])
            allowed_return_type = ["int", "float", "short", "bool"]
            if getters:
                for getter in getters:
                    if str(getter.return_type) in allowed_return_type:
                        getter_args = {
                            "name": str(getter.name),
                            "dtype": str(getter.return_type),
                            "read_only": False
                        }
                        if getter_args["name"] in getter_arguments:
                            getter_args["read_only"] = True
                        self.parsed_data['properties'].append(
                            getter_args.copy())
        except:
            self.parsed_data['properties'] = []

        if self.info['cli']:
            if self.info['yaml_confirm']:
                yaml_generator(self.info['yaml_confirm'])

            if self.info['json_confirm']:
                json_generator(self.info['json_confirm'])
        return self.parsed_data

    def get_documentation(self):
        """
        : Returns the documentation of the header file as python list
        : arguments: file path of the header
        """
        if not self.info['cli']:
            self.validate()
            pattern = re.compile(r'//.*?$|/\*.*?\*/', re.DOTALL | re.MULTILINE)
            header_file = codecs.open(self.info['target_file'], 'r', 'cp932')
            lines = header_file.read()
            header_file.close()
            documentation = pattern.findall(lines)[2:]
            return documentation

    def run(self):
        """ Run, run, run. """
        self.validate()
        self.get_header_info()
