#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" Automatically create YAML bindings for GRC from block code """


import os
import re
import glob
import logging
import yaml

from collections import OrderedDict

try:
    from yaml import CLoader as Loader, CDumper as Dumper
except:
    from yaml import Loader, Dumper

try:
    from gnuradio.blocktool.core import Constants
except ImportError:
    have_blocktool = False
else:
    have_blocktool = True

from ..tools import ParserCCBlock, CMakeFileEditor, ask_yes_no, GRCYAMLGenerator
from .base import ModTool, ModToolException


logger = logging.getLogger(__name__)

## setup dumper for dumping OrderedDict ##
_MAPPING_TAG = yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG


def dict_representer(dumper, data):
    """ Representer to represent special OrderedDict """
    return dumper.represent_dict(data.items())


def dict_constructor(loader, node):
    """ Construct an OrderedDict for dumping """
    return OrderedDict(loader.construct_pairs(node))


Dumper.add_representer(OrderedDict, dict_representer)
Loader.add_constructor(_MAPPING_TAG, dict_constructor)


class ModToolMakeYAML(ModTool):
    """ Make YAML file for GRC block bindings """
    name = 'makeyaml'
    description = 'Generate YAML files for GRC block bindings.'

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
        logger.warning(
            "Warning: This is an experimental feature. Don't expect any magic.")
        # 1) Go through lib/
        if not self.skip_subdirs['lib']:
            if self.info['version'] in ('37', '38', '310'):
                files = self._search_files('lib', '*_impl.cc')
            else:
                files = self._search_files('lib', '*.cc')
            for f in files:
                if os.path.basename(f)[0:2] == 'qa':
                    continue
                (params, iosig, blockname) = self._parse_cc_h(f)
                self._make_grc_yaml_from_block_data(params, iosig, blockname)
        # 2) Go through python/
        # TODO

    def _search_files(self, path, path_glob):
        """ Search for files matching pattern in the given path. """
        files = sorted(glob.glob(f"{path}/{path_glob}"))
        files_filt = []
        logger.info(f"Searching for matching files in {path}/:")
        if self.info['blockname']:
            # ensure the blockname given is not confused with similarly named blocks
            blockname_pattern = ''
            if path == 'python':
                blockname_pattern = f"^(qa_)?{self.info['blockname']}.py$"
            elif path == 'python/bindings':
                blockname_pattern = f"^{self.info['blockname']}_python.cc$"
            elif path == 'python/bindings/docstrings':
                blockname_pattern = f"^{self.info['blockname']}_pydoc_template.h$"
            elif path == 'lib':
                blockname_pattern = f"^{self.info['blockname']}_impl(\\.h|\\.cc)$"
            elif path == self.info['includedir']:
                blockname_pattern = f"^{self.info['blockname']}.h$"
            elif path == 'grc':
                blockname_pattern = f"^{self.info['modname']}_{self.info['blockname']}.block.yml$"
            for f in files:
                if re.search(blockname_pattern, os.path.basename(f)) is not None:
                    files_filt.append(f)
        elif self.info['pattern']:
            # search for block names matching a given regex pattern
            for f in files:
                if re.search(self.info['pattern'], os.path.basename(f)) is not None:
                    files_filt.append(f)
        if len(files_filt) == 0:
            logger.info("None found.")
        return files_filt

    def _make_grc_yaml_from_block_data(self, params, iosig, blockname):
        """ Take the return values from the parser and call the YAML
        generator. Also, check the makefile if the .yml file is in there.
        If necessary, add. """
        fname_yml = f'{self.info["modname"]}_{blockname}.block.yml'
        path_to_yml = os.path.join('grc', fname_yml)
        # Some adaptions for the GRC
        for inout in ('in', 'out'):
            if iosig[inout]['max_ports'] == '-1':
                iosig[inout]['max_ports'] = f'$num_{inout}puts'
                params.append({'key': f'num_{inout}puts',
                               'type': 'int',
                               'name': f'Num {inout}puts',
                               'default': '2',
                               'in_constructor': False})
        file_exists = False
        if os.path.isfile(path_to_yml):
            if not self.info['yes']:
                if not ask_yes_no('Overwrite existing GRC file?', False):
                    return
            else:
                file_exists = True
                logger.warning("Warning: Overwriting existing GRC file.")
        grc_generator = GRCYAMLGenerator(
            modname=self.info['modname'],
            blockname=blockname,
            params=params,
            iosig=iosig
        )
        grc_generator.save(path_to_yml)
        if file_exists:
            self.scm.mark_files_updated((path_to_yml,))
        else:
            self.scm.add_files((path_to_yml,))
        if not self.skip_subdirs['grc']:
            ed = CMakeFileEditor(self._file['cmgrc'])
            if re.search(fname_yml, ed.cfile) is None and not ed.check_for_glob('*.yml'):
                logger.info("Adding GRC bindings to grc/CMakeLists.txt...")
                ed.append_value('install', fname_yml,
                                to_ignore_end='DESTINATION[^()]+')
                ed.write()
                self.scm.mark_files_updated(self._file['cmgrc'])

    def _parse_cc_h(self, fname_cc):
        """ Go through a .cc and .h-file defining a block and return info """
        def _type_translate(p_type, default_v=None):
            """ Translates a type from C++ to GRC """
            translate_dict = {'float': 'float',
                              'double': 'real',
                              'int': 'int',
                              'gr_complex': 'complex',
                              'char': 'byte',
                              'unsigned char': 'byte',
                              'std::string': 'string',
                              'std::vector<int>': 'int_vector',
                              'std::vector<float>': 'real_vector',
                              'std::vector<gr_complex>': 'complex_vector',
                              }
            if p_type in ('int',) and default_v is not None and len(default_v) > 1 and default_v[:2].lower() == '0x':
                return 'hex'
            try:
                return translate_dict[p_type]
            except KeyError:
                return 'raw'

        def _get_blockdata(fname_cc):
            """ Return the block name and the header file name from the .cc file name """
            blockname = os.path.splitext(os.path.basename(
                fname_cc.replace('_impl.', '.')))[0]
            fname_h = (blockname + '.h').replace('_impl.', '.')
            contains_modulename = blockname.startswith(
                self.info['modname'] + '_')
            blockname = blockname.replace(self.info['modname'] + '_', '', 1)
            return (blockname, fname_h, contains_modulename)
        # Go, go, go
        logger.info(f"Making GRC bindings for {fname_cc}...")
        (blockname, fname_h, contains_modulename) = _get_blockdata(fname_cc)
        try:
            parser = ParserCCBlock(fname_cc,
                                   os.path.join(
                                       self.info['includedir'], fname_h),
                                   blockname,
                                   self.info['version'],
                                   _type_translate
                                   )
        except IOError:
            raise ModToolException(
                f"Can't open some of the files necessary to parse {fname_cc}.")

        if contains_modulename:
            return (parser.read_params(), parser.read_io_signature(), self.info['modname'] + '_' + blockname)
        else:
            return (parser.read_params(), parser.read_io_signature(), blockname)


def yaml_generator(self, **kwargs):
    """
    Generate YAML file from the block header file using blocktool API
    """
    header = self.filename.split('.')[0]
    block = self.modname.split('-')[-1]
    label = header.split('_')
    del label[-1]
    yml_file = os.path.join('.', block + '_' + header + '.block.yml')
    _header = (('id', f'{block}_{ header}'),
               ('label', ' '.join(label).upper()),
               ('category', f'[{block.capitalize()}]'),
               ('flags', '[python, cpp]')
               )
    params_list = [
        '${' + s['name'] + '}' for s in self.parsed_data['properties'] if self.parsed_data['properties']]
    str_ = ', '.join(params_list)
    _templates = [('imports', f'from gnuradio import {block}'),
                  ('make', f'{block}.{ header}({str_})')
                  ]

    if self.parsed_data['methods']:
        list_callbacks = []
        for param in self.parsed_data['methods']:
            arguments = []
            for args in param['arguments_type']:
                arguments.append(args['name'])
            arg_list = ['${' + s + '}' for s in arguments if arguments]
            arg_ = ', '.join(arg_list)
            list_callbacks.append(
                param['name'] + f'({arg_})')
        callback_key = ('callbacks')
        callbacks = (callback_key, tuple(list_callbacks))
        _templates.append(callbacks)
    _templates = tuple(_templates)

    data = OrderedDict()
    for tag, value in _header:
        data[tag] = value

    templates = OrderedDict()
    for tag, value in _templates:
        templates[tag] = value
    data['templates'] = templates

    parameters = []
    for param in self.parsed_data['properties']:
        parameter = OrderedDict()
        parameter['id'] = param['name']
        parameter['label'] = param['name'].capitalize()
        parameter['dtype'] = param['dtype']
        parameter['read_only'] = param['read_only']
        parameters.append(parameter)
    if parameters:
        data['parameters'] = parameters

    input_signature = []
    max_input_port = self.parsed_data['io_signature']['input']['max_streams']
    i_sig = self.parsed_data['io_signature']['input']['signature']
    for port in range(0, int(max_input_port)):
        input_sig = OrderedDict()
        if i_sig is Constants.MAKE:
            input_sig['domain'] = 'stream'
            input_sig['dtype'] = self.parsed_data['io_signature']['input']['sizeof_stream_item']
        elif i_sig is Constants.MAKE2:
            input_sig['domain'] = 'stream'
            input_sig['dtype'] = self.parsed_data['io_signature']['input']['sizeof_stream_item' +
                                                                           str(port + 1)]
        elif i_sig is Constants.MAKE3:
            input_sig['domain'] = 'stream'
            input_sig['dtype'] = self.parsed_data['io_signature']['input']['sizeof_stream_item' +
                                                                           str(port + 1)]
        elif i_sig is Constants.MAKEV:
            input_sig['domain'] = 'stream'
            input_sig['dtype'] = self.parsed_data['io_signature']['input']['sizeof_stream_items']
        input_signature.append(input_sig)

    if self.parsed_data['message_port']['input']:
        for _input in self.parsed_data['message_port']['input']:
            m_input_sig = OrderedDict()
            m_input_sig['domain'] = 'message'
            m_input_sig['id'] = _input
            input_signature.append(m_input_sig)
    if input_signature:
        data['inputs'] = input_signature

    output_signature = []
    max_output_port = self.parsed_data['io_signature']['output']['max_streams']
    o_sig = self.parsed_data['io_signature']['output']['signature']
    for port in range(0, int(max_output_port)):
        output_sig = OrderedDict()
        if o_sig is Constants.MAKE:
            output_sig['domain'] = 'stream'
            output_sig['dtype'] = self.parsed_data['io_signature']['output']['sizeof_stream_item']
        elif o_sig is Constants.MAKE2:
            output_sig['domain'] = 'stream'
            output_sig['dtype'] = self.parsed_data['io_signature']['output']['sizeof_stream_item' +
                                                                             str(port + 1)]
        elif o_sig is Constants.MAKE3:
            output_sig['domain'] = 'stream'
            output_sig['dtype'] = self.parsed_data['io_signature']['output']['sizeof_stream_item' +
                                                                             str(port + 1)]
        elif o_sig is Constants.MAKEV:
            output_sig['domain'] = 'stream'
            output_sig['dtype'] = self.parsed_data['io_signature']['output']['sizeof_stream_items']
        output_signature.append(output_sig)

    if self.parsed_data['message_port']['output']:
        for _output in self.parsed_data['message_port']['output']:
            m_output_sig = OrderedDict()
            m_output_sig['domain'] = 'message'
            m_output_sig['id'] = _output
            output_signature.append(m_output_sig)
    if output_signature:
        data['outputs'] = output_signature

    param_ = ', '.join(params_list)
    _cpp_templates = [('includes', '#include <gnuradio/{block}/{self.filename}>'),
                      ('declarations', '{block}::{ header}::sptr ${{id}}'),
                      ('make',
                       'this->${{id}} = {block}::{ header}::make({param_})')
                      ]

    if self.parsed_data['methods']:
        list_callbacks = []
        for param in self.parsed_data['methods']:
            arguments = []
            for args in param['arguments_type']:
                arguments.append(args['name'])
            arg_list = ['${' + s + '}' for s in arguments if arguments]
            arg_ = ', '.join(arg_list)
            list_callbacks.append(
                param['name'] + f'({arg_})')
        callback_key = ('callbacks')
        callbacks = (callback_key, tuple(list_callbacks))
        _cpp_templates.append(callbacks)

    link = ('link', 'gnuradio-{block}')
    _cpp_templates.append(link)
    _cpp_templates = tuple(_cpp_templates)

    cpp_templates = OrderedDict()
    for tag, value in _cpp_templates:
        cpp_templates[tag] = value
    data['cpp_templates'] = cpp_templates

    if self.parsed_data['docstring'] is not None:
        data['documentation'] = self.parsed_data['docstring']
    data['file_format'] = 1

    if kwargs['output']:
        with open(yml_file, 'w') as yml:
            yaml.dump(data, yml, Dumper=Dumper, default_flow_style=False)
    else:
        print(yaml.dump(data, Dumper=Dumper, allow_unicode=True,
                        default_flow_style=False, indent=4))
