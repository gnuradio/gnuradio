#
# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
""" A tool for generating YAML bindings """


from collections import OrderedDict

import yaml
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except:
    from yaml import Loader, Dumper

from .util_functions import is_number


## setup dumper for dumping OrderedDict ##
_mapping_tag = yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG


def dict_representer(dumper, data):
    """ Representer to represent special OrderedDict """
    return dumper.represent_dict(data.items())


def dict_constructor(loader, node):
    """ Construct an OrderedDict for dumping """
    return OrderedDict(loader.construct_pairs(node))


Dumper.add_representer(OrderedDict, dict_representer)
Loader.add_constructor(_mapping_tag, dict_constructor)


class GRCYAMLGenerator(object):
    """ Create and write the YAML bindings for a GRC block. """

    def __init__(self, modname=None, blockname=None, doc=None, params=None, iosig=None):
        """docstring for __init__"""
        params_list = [
            '${' + s['key'] + '}' for s in params if s['in_constructor']]
        # Can't make a dict 'cause order matters
        str_ = ', '.join(params_list)
        self._header = (('id', f'{modname}_{blockname}'),
                        ('label', blockname.replace('_', ' ')),
                        (f'category', f'[{modname.capitalize()}]')
                        )
        self._templates = (('imports', f'from gnuradio import {modname}'),
                           ('make', f'{modname}.{blockname}({str_})')
                           )
        self.params = params
        self.iosig = iosig
        self.doc = doc
        self.data = None

    def make_yaml(self):
        """ Create the actual tag tree """
        data = OrderedDict()
        for tag, value in self._header:
            data[tag] = value

        templates = OrderedDict()
        for tag, value in self._templates:
            templates[tag] = value

        data['templates'] = templates

        parameters = []
        for param in self.params:
            parameter = OrderedDict()
            parameter['id'] = param['key']
            parameter['label'] = param['key'].capitalize()
            if param['default']:
                parameter['default'] = param['default']
            parameter['dtype'] = param['type']
            parameters.append(parameter)

        if parameters:
            data['parameters'] = parameters

        inputs = []
        outputs = []
        iosig = self.iosig
        for inout in sorted(iosig.keys()):
            if iosig[inout]['max_ports'] == '0':
                continue
            for i in range(len(iosig[inout]['type'])):
                s_type = {'in': 'input', 'out': 'output'}[inout]
                s_obj = OrderedDict()
                s_obj['label'] = inout
                s_obj['domain'] = 'stream'
                s_obj['dtype'] = iosig[inout]['type'][i]
                if iosig[inout]['vlen'][i] != '1':
                    vlen = iosig[inout]['vlen'][i]
                    if is_number(vlen):
                        s_obj['vlen'] = int(vlen)
                    else:
                        s_obj['vlen'] = '${ ' + vlen + ' }'
                if i == len(iosig[inout]['type']) - 1:
                    if not is_number(iosig[inout]['max_ports']):
                        s_obj['multiplicity'] = iosig[inout]['max_ports']
                    elif len(iosig[inout]['type']) < int(iosig[inout]['max_ports']):
                        s_obj['multiplicity'] = int(iosig[inout]['max_ports']) - len(iosig[inout]['type']) + 1
                if s_type == 'input':
                    inputs.append(s_obj)
                elif s_type == 'output':
                    outputs.append(s_obj)

        if inputs:
            data['inputs'] = inputs

        if outputs:
            data['outputs'] = outputs

        if self.doc is not None:
            data['documentation'] = self.doc
        self.data = data
        data['file_format'] = 1

    def save(self, filename):
        """ Write the YAML file """
        self.make_yaml()
        with open(filename, 'w') as f:
            yaml.dump(self.data, f, Dumper=Dumper, default_flow_style=False)
