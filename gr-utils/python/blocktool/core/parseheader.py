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

from pygccxml import parser, declarations, utils

from blocktool.core.base import BlockToolException, BlockTool
from blocktool.core.iosignature import io_signature
from blocktool.core import Constants

LOGGER = logging.getLogger(__name__)


class BlockHeaderParser(BlockTool):
    """
    : Single argument required: file_path
    file_path: enter path for the header block in any of GNU Radio module
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
        if list_dir.startswith(Constants.GR):
            list_dir = list_dir.split('-')[-1]
            module_types.append(list_dir)
    module_types.remove(Constants.UTILS)
    module_types = tuple(module_types)
    parsed_data = {}

    def __init__(self, file_path=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        open(file_path, 'r')
        file_path = os.path.abspath(file_path)
        self.target_file = file_path
        self.modname = os.path.basename(os.path.dirname(
            os.path.dirname(os.path.dirname(os.path.dirname(file_path)))))
        self.filename = os.path.basename(file_path)
        self.targetdir = os.path.dirname(file_path)
        self.impldir = os.path.abspath(os.path.join(file_path,
                                                             '..', '..', '..', '..', 'lib'))
        self.impl_file = os.path.join(self.impldir,
                                              self.filename.split('.')[0]+'_impl.cc')
        self.validate()

    def validate(self):
        """ Override the Blocktool validate function """
        BlockTool._validate(self)
        if self.modname.startswith(Constants.GR):
            _module = self.modname.split('-')[-1]
        else:
            _module = self.modname
            self.modname = Constants.GR+_module
        if _module not in self.module_types:
            raise BlockToolException('Module must be one of {}.'.format(
                ', '.join(self.module_types)))
        if not self.filename.endswith('.h'):
            raise BlockToolException(
                'Invalid header file')
        _target_dir = os.path.join(self.target_dir,
                                   self.modname,
                                   'include',
                                   'gnuradio',
                                   self.modname.split('-')[-1])
        if _target_dir != self.targetdir:
            raise BlockToolException('Invalid GNU Radio module')
        open(self.impl_file, 'r')

    def get_header_info(self):
        """
        PyGCCXML header code parser
        magic happens here!
        # : returns the parsed header data in python dict
        # : return dict keys: namespace, class, io_signature, make,
        #                properties, methods
        # : Can be used as an CLI command or an extenal API
        """
        gr = self.modname.split('-')[0]
        module = self.modname.split('-')[-1]
        generator_path, generator_name = utils.find_xml_generator()
        xml_generator_config = parser.xml_generator_configuration_t(
            xml_generator_path=generator_path,
            xml_generator=generator_name,
            compiler='gcc')
        decls = parser.parse(
            [self.target_file], xml_generator_config)
        global_namespace = declarations.get_global_namespace(decls)

        # namespace
        try:
            self.parsed_data['namespace'] = []
            ns = global_namespace.namespace(gr)
            if ns is None:
                raise BlockToolException
            main_namespace = ns.namespace(module)
            if main_namespace is None:
                raise BlockToolException('namespace cannot be none')
            self.parsed_data['namespace'] = [gr, module]
            if main_namespace.declarations:
                for _namespace in main_namespace.declarations:
                    if isinstance(_namespace, declarations.namespace_t):
                        if Constants.KERNEL not in str(_namespace):
                            main_namespace = _namespace
                            self.parsed_data['namespace'].append(
                                str(_namespace).split('::')[-1].split(' ')[0])
        except BlockToolException as exception:
            raise BlockToolException(
                'Must be a header with block api!\n{}'.format(exception))

        # class
        try:
            self.parsed_data['class'] = ''
            for _class in main_namespace.declarations:
                if isinstance(_class, declarations.class_t):
                    main_class = _class
                    self.parsed_data['class'] = str(_class).split('::')[
                        2].split(' ')[0]
        except BlockToolException as exception:
            raise BlockToolException(
                'A block header always has a class!\n{}'.format(exception))

        # io_signature
        try:
            self.parsed_data['io_signature'] = {}
            self.parsed_data['io_signature'] = io_signature(
                self.impl_file)
        except BlockToolException as exception:
            raise BlockToolException(
                'A block header always has a io_signature!\n{}'.format(exception))

        # make
        try:
            self.parsed_data['make'] = {}
            self.parsed_data['make']['arguments'] = []
            query_m = declarations.custom_matcher_t(
                lambda mem_fun: mem_fun.name.startswith('make'))
            query_make = query_m & declarations.access_type_matcher_t('public')
            make_func = main_class.member_functions(function=query_make,
                                                    allow_empty=True,
                                                    header_file=self.target_file)
            criteria = declarations.calldef_matcher(name='make')
            _make_fun = declarations.matcher.get_single(criteria, main_class)
            _make_fun = str(_make_fun).split(
                'make')[-1].split(')')[0].split('(')[1].lstrip().rstrip().split(',')
            if make_func:
                for arg in make_func[0].arguments:
                    for _arg in _make_fun:
                        if str(arg.name) in _arg:
                            make_arguments = {
                                "name": str(arg.name),
                                "dtype": str(arg.decl_type),
                                "default": ""
                            }
                            if re.findall(r'[-+]?\d*\.\d+|\d+', _arg):
                                make_arguments['default'] = re.findall(
                                    r'[-+]?\d*\.\d+|\d+', _arg)[0]
                            elif re.findall(r'\"(.+?)\"', _arg):
                                make_arguments['default'] = re.findall(
                                    r'\"(.+?)\"', _arg)[0]
                            elif "true" in _arg:
                                make_arguments['default'] = "True"
                            elif "false" in _arg:
                                make_arguments['default'] = "False"
                    self.parsed_data['make']['arguments'].append(
                        make_arguments.copy())
        except BlockToolException as exception:
            raise BlockToolException(
                'A block API always has a factory signature!\n{}'.format(exception))

        # setters
        try:
            self.parsed_data['methods'] = []
            query_methods = declarations.access_type_matcher_t('public')
            setters = main_class.member_functions(function=query_methods,
                                                  allow_empty=True,
                                                  header_file=self.target_file)
            getter_arguments = []
            if setters:
                for setter in setters:
                    if str(setter.name).startswith('set_') and setter.arguments:
                        setter_args = {
                            "name": str(setter.name),
                            "arguments_type": []
                        }
                        for argument in setter.arguments:
                            args = {
                                "name": str(argument.name),
                                "dtype": str(argument.decl_type)
                            }
                            getter_arguments.append(args['name'])
                            setter_args['arguments_type'].append(args.copy())
                        self.parsed_data['methods'].append(setter_args.copy())
            else:
                self.parsed_data['methods'] = []
        except BlockToolException as exception:
            raise exception

        # getters
        try:
            self.parsed_data['properties'] = []
            query_properties = declarations.access_type_matcher_t('public')
            getters = main_class.member_functions(function=query_properties,
                                                  allow_empty=True,
                                                  header_file=self.target_file)
            if getters:
                for getter in getters:
                    if not getter.arguments or getter.has_const:
                        getter_args = {
                            "name": str(getter.name),
                            "dtype": str(getter.return_type),
                            "read_only": True
                        }
                        if getter_args['name'] in getter_arguments:
                            getter_args["read_only"] = False
                        self.parsed_data['properties'].append(
                            getter_args.copy())
            else:
                self.parsed_data['properties'] = []
        except BlockToolException as exception:
            raise exception

        # documentation
        try:
            header_file = codecs.open(self.target_file, 'r', 'cp932')
            self.parsed_data['docstring'] = re.compile(
                r'//.*?$|/\*.*?\*/', re.DOTALL | re.MULTILINE).findall(
                    header_file.read())[2:]
            header_file.close()
        except:
            self.parsed_data['docstring'] = []

        return self.parsed_data

    def run(self):
        """ Run, run, run. """
        self.get_header_info()
