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
""" Module to generate YAML file from the parsed data """

from __future__ import print_function
from __future__ import absolute_import
from __future__ import unicode_literals

from collections import OrderedDict

import os
import click
import yaml
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except:
    from yaml import Loader, Dumper

from blocktool.cli.base import run
from blocktool.core.parseheader import BlockHeaderParser

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


@click.command('makeyaml', short_help='Generate a YAML file from a block header')
@click.argument('file-path', nargs=1)
def cli(**kwargs):
    """
    \b
    Get parsed output for a header file from GNU Radio module
    """
    self = BlockHeaderParser(**kwargs)
    self.cli = True
    self.yaml_confirm = True
    click.secho('Header file: {}'.format(self.filename), fg='green')
    run(self)
    yaml_generator(self)


def yaml_generator(self):
    """
    Generate YAML file from the block header file
    """
    header = self.filename.split('.')[0]
    block = self.modname.split('-')[-1]
    click.secho('Successfully generated {}_{}.yml'.format(
        block, header), fg='green')
    label = header.split('_')
    del label[-1]
    yml_file = os.path.join('.', block+'_'+header+'.yml')
    _header = (('id', '{}_{}'.format(block, header)),
               ('label', ' '.join(label).upper()),
               ('category', '[{}]'.format(block.capitalize())),
               ('flags', '[python, cpp]')
               )
    params_list = [
        '${'+s['name']+'}' for s in self.parsed_data['properties'] if self.parsed_data['properties']]
    _templates = [('imports', 'from gnuradio import {}'.format(block)),
                  ('make', '{}.{}({})'.format(block, header, ', '.join(params_list)))
                  ]

    if self.parsed_data['methods']:
        list_callbacks = []
        for param in self.parsed_data['methods']:
            arguments = []
            for args in param['arguments_type']:
                arguments.append(args['name'])
            arg_list = ['${'+s+'}' for s in arguments if arguments]
            list_callbacks.append(
                param['name']+'({})'.format(', '.join(arg_list)))
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

    data['input'] = self.parsed_data['io_signature']['input']
    data['output'] = self.parsed_data['io_signature']['output']

    _cpp_templates = [('includes', '#include <gnuradio/{}/{}>'.format(block, self.filename)),
                  ('declartions', '{}::{}::sptr ${{id}}'.format(block, header)),
                  ('make', 'this->${{id}} = {}::{}::make({})'.format(block, header, ', '.join(params_list)))
                  ]

    if self.parsed_data['methods']:
        list_callbacks = []
        for param in self.parsed_data['methods']:
            arguments = []
            for args in param['arguments_type']:
                arguments.append(args['name'])
            arg_list = ['${'+s+'}' for s in arguments if arguments]
            list_callbacks.append(
                param['name']+'({})'.format(', '.join(arg_list)))
        callback_key = ('callbacks')
        callbacks = (callback_key, tuple(list_callbacks))
        _cpp_templates.append(callbacks)

    link = ('link', 'gnuradio-{}'.format(block))
    _cpp_templates.append(link)
    _cpp_templates = tuple(_cpp_templates)

    cpp_templates = OrderedDict()
    for tag, value in _cpp_templates:
        cpp_templates[tag] = value
    data['cpp_templates'] = cpp_templates

    if self.parsed_data['docstring'] is not None:
        data['documentation'] = self.parsed_data['docstring']
    data['file_format'] = 1

    with open(yml_file, 'w') as yml:
        yaml.dump(data, yml, Dumper=Dumper, default_flow_style=False)
