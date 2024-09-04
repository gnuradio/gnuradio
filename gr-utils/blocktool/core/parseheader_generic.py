#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to generate AST for the headers and parse it """


import os
import re
import codecs
import logging

from ..core.base import BlockToolException, BlockTool
from ..core.iosignature import io_signature, message_port
from ..core.comments import read_comments, add_comments, exist_comments
from ..core import Constants

LOGGER = logging.getLogger(__name__)
PYGCCXML_AVAILABLE = False

try:
    from pygccxml import parser, declarations, utils
    PYGCCXML_AVAILABLE = True
except:
    from gnuradio.modtool.tools import ParserCCBlock


class GenericHeaderParser(BlockTool):
    """
    : Single argument required: file_path
    file_path: enter path for the header block in any of GNU Radio module
        : returns the parsed header data in python dict
        : return dict keys: namespace, class, io_signature, make,
                       properties, methods
    : Can be used as an CLI command or an external API
    """
    name = 'Block Parse Header'
    description = 'Create a parsed output from a block header file'

    def __init__(self, file_path=None, blocktool_comments=False, define_symbols=None, include_paths=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        self.parsed_data = {}
        self.addcomments = blocktool_comments
        self.define_symbols = ('BOOST_ATOMIC_DETAIL_EXTRA_BACKEND_GENERIC', 'DISABLE_LOGGER_H')
        if define_symbols:
            self.define_symbols += define_symbols
        self.include_paths = None
        if (include_paths):
            self.include_paths = [p.strip() for p in include_paths.split(',')]
        if not os.path.isfile(file_path):
            raise BlockToolException('file', file_path, 'does not exist')
        file_path = os.path.abspath(file_path)
        self.target_file = file_path

        self.initialize()
        self.validate()

    def initialize(self):
        """
        initialize all the required API variables
        """

        if type(self.target_file) == list:
            self.module = self.target_file
            for dirs in self.target_file:
                if not os.path.basename(self.module).startswith(Constants.GR):
                    self.module = os.path.abspath(
                        os.path.join(self.module, os.pardir))
        else:
            self.module = self.target_file
            if not os.path.basename(self.module).startswith(Constants.GR):
                self.module = os.path.abspath(
                    os.path.join(self.module, os.pardir))

        self.modname = os.path.basename(self.module)
        self.filename = os.path.basename(self.target_file)
        self.targetdir = os.path.dirname(self.target_file)

    def validate(self):
        """ Override the Blocktool validate function """
        BlockTool._validate(self)
        if not self.filename.endswith('.h'):
            raise BlockToolException(
                'Cannot parse a non-header file')

    def _parse_cc_h(self, fname_h):
        """ Go through a .cc and .h-file defining a block and return info """
        def _get_blockdata(fname_h):
            """ Return the block name and the header file name from the .cc file name """
            blockname = os.path.splitext(os.path.basename(
                fname_h))[0]
            fname_cc = blockname + '_impl' + '.cc'
            contains_modulename = blockname.startswith(
                self.modname + '_')
            blockname = blockname.replace(self.modname + '_', '', 1)
            fname_cc = os.path.join(fname_h.split(
                'include')[0], 'lib', fname_cc)
            return (blockname, fname_cc, contains_modulename)
        # Go, go, go
        LOGGER.info("Making GRC bindings for {}...".format(fname_h))
        (blockname, fname_cc, contains_modulename) = _get_blockdata(fname_h)
        try:
            parser = ParserCCBlock(fname_cc,
                                   os.path.join(
                                       self.targetdir, fname_h),
                                   blockname,
                                   '39'
                                   )
        except IOError:
            raise BlockToolException(
                "Can't open some of the files necessary to parse {}.".format(fname_cc))

        if contains_modulename:
            return (parser.read_params(), parser.read_io_signature(), self.modname + '_' + blockname)
        else:
            return (parser.read_params(), parser.read_io_signature(), blockname)

    def parse_function(self, func_decl):
        fcn_dict = {
            "name": str(func_decl.name),
            "return_type": str(func_decl.return_type),
            "has_static": func_decl.has_static if hasattr(func_decl, 'has_static') else '0',
            "arguments": []
        }
        for argument in func_decl.arguments:
            args = {
                "name": str(argument.name),
                "dtype": str(argument.decl_type),
                "default": argument.default_value
            }
            fcn_dict['arguments'].append(args)

        return fcn_dict

    def parse_class(self, class_decl):
        class_dict = {'name': class_decl.name, 'member_functions': []}
        if class_decl.bases:
            class_dict['bases'] = class_decl.bases[0].declaration_path

        constructors = []
        # constructors
        constructors = []
        query_methods = declarations.access_type_matcher_t('public')
        if hasattr(class_decl, 'constructors'):
            cotrs = class_decl.constructors(function=query_methods,
                                            allow_empty=True, recursive=False,
                                            header_file=self.target_file,
                                            name=class_decl.name)
            for cotr in cotrs:
                constructors.append(self.parse_function(cotr))

        class_dict['constructors'] = constructors

        # class member functions
        member_functions = []
        query_methods = declarations.access_type_matcher_t('public')
        if hasattr(class_decl, 'member_functions'):
            functions = class_decl.member_functions(function=query_methods,
                                                    allow_empty=True, recursive=False,
                                                    header_file=self.target_file)
            for fcn in functions:
                if str(fcn.name) not in [class_decl.name, '~' + class_decl.name]:
                    member_functions.append(self.parse_function(fcn))

        class_dict['member_functions'] = member_functions

        # enums
        class_enums = []
        if hasattr(class_decl, 'variables'):
            enums = class_decl.enumerations(
                allow_empty=True, recursive=False, header_file=self.target_file)
            if enums:
                for _enum in enums:
                    current_enum = {'name': _enum.name, 'values': _enum.values}
                    class_enums.append(current_enum)

        class_dict['enums'] = class_enums

        # variables
        class_vars = []
        query_methods = declarations.access_type_matcher_t('public')
        if hasattr(class_decl, 'variables'):
            variables = class_decl.variables(allow_empty=True, recursive=False, function=query_methods,
                                             header_file=self.target_file)
            if variables:
                for _var in variables:
                    current_var = {
                        'name': _var.name, 'value': _var.value, "has_static": _var.has_static if hasattr(_var, 'has_static') else '0'}
                    class_vars.append(current_var)
        class_dict['vars'] = class_vars

        return class_dict

    def parse_namespace(self, namespace_decl):
        namespace_dict = {}
        # enums
        namespace_dict['name'] = namespace_decl.decl_string
        namespace_dict['enums'] = []
        if hasattr(namespace_decl, 'enumerations'):
            enums = namespace_decl.enumerations(
                allow_empty=True, recursive=False, header_file=self.target_file)
            if enums:
                for _enum in enums:
                    current_enum = {'name': _enum.name, 'values': _enum.values}
                    namespace_dict['enums'].append(current_enum)

        # variables
        namespace_dict['variables'] = []
        if hasattr(namespace_decl, 'variables'):
            variables = namespace_decl.variables(
                allow_empty=True, recursive=False, header_file=self.target_file)
            if variables:
                for _var in variables:
                    current_var = {
                        'name': _var.name, 'values': _var.value, 'has_static': _var.has_static if hasattr(_var, 'has_static') else '0'}
                    namespace_dict['variables'].append(current_var)

        # classes
        namespace_dict['classes'] = []
        if hasattr(namespace_decl, 'classes'):
            classes = namespace_decl.classes(
                allow_empty=True, recursive=False, header_file=self.target_file)
            if classes:
                for _class in classes:
                    namespace_dict['classes'].append(self.parse_class(_class))

        # free functions
        namespace_dict['free_functions'] = []
        free_functions = []
        if hasattr(namespace_decl, 'free_functions'):
            functions = namespace_decl.free_functions(allow_empty=True, recursive=False,
                                                      header_file=self.target_file)
            for fcn in functions:
                if str(fcn.name) not in ['make']:
                    free_functions.append(self.parse_function(fcn))

            namespace_dict['free_functions'] = free_functions

        # sub namespaces
        namespace_dict['namespaces'] = []

        if hasattr(namespace_decl, 'namespaces'):
            sub_namespaces = []
            sub_namespaces_decl = namespace_decl.namespaces(allow_empty=True)
            for ns in sub_namespaces_decl:
                sub_namespaces.append(self.parse_namespace(ns))

            namespace_dict['namespaces'] = sub_namespaces

        return namespace_dict

    def get_header_info(self, namespace_to_parse):
        """
        PyGCCXML header code parser
        magic happens here!
        : returns the parsed header data in python dict
        : return dict keys: namespace, class, io_signature, make,
                       properties, methods
        : Can be used as an CLI command or an external API
        """
        module = self.modname.split('-')[-1]
        self.parsed_data['module_name'] = module
        self.parsed_data['filename'] = self.filename

        import hashlib
        hasher = hashlib.md5()
        with open(self.target_file, 'rb') as file_in:
            buf = file_in.read()
            hasher.update(buf)
        self.parsed_data['md5hash'] = hasher.hexdigest()

        # Right now if pygccxml is not installed, it will only handle the make function
        # TODO: extend this to other publicly declared functions in the h file
        if not PYGCCXML_AVAILABLE:
            self.parsed_data['parser'] = 'simple'
            (params, iosig, blockname) = self._parse_cc_h(self.target_file)
            self.parsed_data['target_namespace'] = namespace_to_parse

            namespace_dict = {}
            namespace_dict['name'] = "::".join(namespace_to_parse)
            class_dict = {}
            class_dict['name'] = blockname
            class_dict['bases'] = ["::", "gr", "block"]

            mf_dict = {
                "name": "make",
                "return_type": "::".join(namespace_to_parse + [blockname, "sptr"]),
                "has_static": "1"
            }

            args = []

            for p in params:
                arg_dict = {
                    "name": p['key'],
                    "dtype": p['type'],
                    "default": p['default']
                }
                args.append(arg_dict)

            mf_dict["arguments"] = args

            class_dict["member_functions"] = [mf_dict]
            namespace_dict["classes"] = [class_dict]
            self.parsed_data["namespace"] = namespace_dict

            return self.parsed_data
        else:
            self.parsed_data['parser'] = 'pygccxml'
            generator_path, generator_name = utils.find_xml_generator()
            xml_generator_config = parser.xml_generator_configuration_t(
                xml_generator_path=generator_path,
                xml_generator=generator_name,
                include_paths=self.include_paths,
                compiler='g++',
                undefine_symbols=['__PIE__'],
                define_symbols=self.define_symbols,
                # There seems to be a bug where on some platforms, GCC fails because it finds the
                # _GLIBCXX_OPERATOR_DELETE and considers it a "non-usual deallocation function".
                # Even if that is inherent to C++17. Anyways, in
                # https://github.com/gnuradio/gnuradio/issues/6477 user boatbod convinces us that
                # adding -fsized_deallocations helps this. Neither we nor the GCC mailing lists
                # are perfectly sure why that is, but it does help. Since this is just for binding,
                # none of the thus-generated code ends up anywhere, so here we go:
                cflags='-std=c++17 -fPIC -fsized-deallocation')
            decls = parser.parse(
                [self.target_file], xml_generator_config,
                compilation_mode=parser.COMPILATION_MODE.ALL_AT_ONCE)

            global_namespace = declarations.get_global_namespace(decls)

            # namespace
            # try:
            main_namespace = global_namespace
            for ns in namespace_to_parse:
                main_namespace = main_namespace.namespace(ns)
            if main_namespace is None:
                raise BlockToolException('namespace cannot be none')
            self.parsed_data['target_namespace'] = namespace_to_parse

            self.parsed_data['namespace'] = self.parse_namespace(
                main_namespace)

            # except RuntimeError:
            #     raise BlockToolException(
            #         'Invalid namespace format in the block header file')

            # namespace

            return self.parsed_data

    def run_blocktool(self):
        """ Run, run, run. """
        pass
        # self.get_header_info()
