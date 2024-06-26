#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Module to generate AST for the headers and parse it """


from ..core import Constants
from ..core.comments import read_comments, add_comments, exist_comments
from ..core.iosignature import io_signature, message_port
from ..core.base import BlockToolException, BlockTool
import os
import re
import codecs
import logging

PYGCCXML_AVAILABLE = False
try:
    from pygccxml import parser, declarations, utils
    PYGCCXML_AVAILABLE = True
except:
    pass


LOGGER = logging.getLogger(__name__)


class BlockHeaderParser(BlockTool):
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

    def __init__(self, file_path=None, blocktool_comments=False, include_paths=None, **kwargs):
        """ __init__ """
        BlockTool.__init__(self, **kwargs)
        self.parsed_data = {}
        self.addcomments = blocktool_comments
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
        self.module = self.target_file
        for dirs in self.module:
            if not os.path.basename(self.module).startswith(Constants.GR):
                self.module = os.path.abspath(
                    os.path.join(self.module, os.pardir))
        self.modname = os.path.basename(self.module)
        self.filename = os.path.basename(self.target_file)
        self.targetdir = os.path.dirname(self.target_file)
        for dirs in os.scandir(self.module):
            if dirs.is_dir():
                if dirs.path.endswith('lib'):
                    self.impldir = dirs.path
        self.impl_file = os.path.join(self.impldir,
                                      self.filename.split('.')[0] + '_impl.cc')

    def validate(self):
        """ Override the Blocktool validate function """
        BlockTool._validate(self)
        if not self.filename.endswith('.h'):
            raise BlockToolException(
                'Cannot parse a non-header file')

    def get_header_info(self):
        """
        PyGCCXML header code parser
        magic happens here!
        : returns the parsed header data in python dict
        : return dict keys: namespace, class, io_signature, make,
                       properties, methods
        : Can be used as an CLI command or an external API
        """
        gr = self.modname.split('-')[0]
        module = self.modname.split('-')[-1]
        self.parsed_data['module_name'] = module
        generator_path, generator_name = utils.find_xml_generator()
        xml_generator_config = parser.xml_generator_configuration_t(
            xml_generator_path=generator_path,
            xml_generator=generator_name,
            include_paths=self.include_paths,
            compiler='gcc',
            define_symbols=['BOOST_ATOMIC_DETAIL_EXTRA_BACKEND_GENERIC'],
            # See the comment parseheader_generic: -fsized-deallocation
            # helps what seems to be a compiler bug (?).
            cflags='-std=c++17 -fPIC -fsized-deallocation')
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
        except RuntimeError:
            raise BlockToolException(
                'Invalid namespace format in the block header file')

        # class
        try:
            self.parsed_data['class'] = ''
            for _class in main_namespace.declarations:
                if isinstance(_class, declarations.class_t):
                    expected_class_name = self.filename.split('.')[0]
                    if expected_class_name in str(_class):
                        main_class = _class
                        self.parsed_data['class'] = str(_class).split('::')[
                            2].split(' ')[0]
                        # in more complicated blocks, there are many classes included in this declaration
                        # Break after the first class - safe to assume this is the "main class"?
                        if len(main_class.bases) > 0:
                            self.parsed_data['block_type'] = main_class.bases[0].declaration_path[-1]
                            break
        except RuntimeError:
            raise BlockToolException(
                'Block header namespace {} must consist of a valid class instance'.format(module))

        # io_signature, message_ports
        self.parsed_data['io_signature'] = {}
        self.parsed_data['message_port'] = {}
        if os.path.isfile(self.impl_file) and exist_comments(self):
            self.parsed_data['io_signature'] = io_signature(
                self.impl_file)
            self.parsed_data['message_port'] = message_port(
                self.impl_file)
            read_comments(self)
        elif os.path.isfile(self.impl_file) and not exist_comments(self):
            self.parsed_data['io_signature'] = io_signature(
                self.impl_file)
            self.parsed_data['message_port'] = message_port(
                self.impl_file)
            if self.addcomments:
                add_comments(self)
        elif not os.path.isfile(self.impl_file) and exist_comments(self):
            read_comments(self)
        else:
            self.parsed_data['io_signature'] = {
                "input": [],
                "output": []
            }
            self.parsed_data['message_port'] = self.parsed_data['io_signature']

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
                    make_arguments = None
                    r'''
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
                    '''
                    # In case the search did not find an argument in the inner loop
                    # This happens while parsing digital/symbol_sync_cc.h
                    if make_arguments:
                        self.parsed_data['make']['arguments'].append(
                            make_arguments.copy())
                    else:
                        self.parsed_data['make']['arguments'].append(
                            {
                                "name": str(arg.name),
                                "dtype": str(arg.decl_type),
                                "default": arg.default_value  # can we get default argument directly from arg
                            })
        except RuntimeError:
            self.parsed_data['make'] = {}
            self.parsed_data['make']['arguments'] = []

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
        except RuntimeError:
            self.parsed_data['methods'] = []

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
        except RuntimeError:
            self.parsed_data['properties'] = []

        # all member functions
        # setters and getters do not return all member functions for a block
        try:
            self.parsed_data['member_functions'] = []
            query_methods = declarations.access_type_matcher_t('public')
            functions = main_class.member_functions(function=query_methods,
                                                    allow_empty=True,
                                                    header_file=self.target_file)
            if functions:
                for fcn in functions:
                    if str(fcn.name) not in [main_class.name, '~' + main_class.name, 'make']:
                        fcn_args = {
                            "name": str(fcn.name),
                            "arguments": []
                        }
                        for argument in fcn.arguments:
                            args = {
                                "name": str(argument.name),
                                "dtype": str(argument.decl_type),
                                "default": argument.default_value
                            }
                            fcn_args['arguments'].append(args.copy())
                        self.parsed_data['member_functions'].append(
                            fcn_args.copy())
        except RuntimeError:
            self.parsed_data['member_functions'] = []

        # documentation
        try:
            _index = None
            header_file = codecs.open(self.target_file, 'r', 'cp932')
            self.parsed_data['docstring'] = re.compile(
                r'//.*?$|/\*.*?\*/', re.DOTALL | re.MULTILINE).findall(
                    header_file.read())[2:]
            header_file.close()
            for doc in self.parsed_data['docstring']:
                if Constants.BLOCKTOOL in doc:
                    _index = self.parsed_data['docstring'].index(doc)
            if _index is not None:
                self.parsed_data['docstring'] = self.parsed_data['docstring'][: _index]
        except:
            self.parsed_data['docstring'] = []

        return self.parsed_data

    def run_blocktool(self):
        """ Run, run, run. """
        self.get_header_info()
