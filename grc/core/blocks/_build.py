# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import itertools
import re

from ..Constants import ADVANCED_PARAM_TAB
from ..utils import to_list
from ..Messages import send_warning

from .block import Block
from ._flags import Flags
from ._templates import MakoTemplates


def build(id, label='', category='', flags='', documentation='',
          value=None, asserts=None,
          parameters=None, inputs=None, outputs=None, templates=None, cpp_templates=None, **kwargs):
    block_id = id

    cls = type(str(block_id), (Block,), {})
    cls.key = block_id

    cls.label = label or block_id.title()
    cls.category = [cat.strip() for cat in category.split('/') if cat.strip()]

    cls.flags = Flags(flags)
    if re.match(r'options$|variable|virtual', block_id):
        cls.flags.set(Flags.NOT_DSP, Flags.DISABLE_BYPASS)

    cls.documentation = {'': documentation.strip('\n\t ').replace('\\\n', '')}

    cls.asserts = [_single_mako_expr(a, block_id) for a in to_list(asserts)]

    cls.inputs_data = build_ports(inputs, 'sink') if inputs else []
    cls.outputs_data = build_ports(outputs, 'source') if outputs else []
    cls.parameters_data = build_params(parameters or [],
                                       bool(cls.inputs_data), bool(cls.outputs_data), cls.flags, block_id)
    cls.extra_data = kwargs

    templates = templates or {}
    cls.templates = MakoTemplates(
        imports=templates.get('imports', ''),
        make=templates.get('make', ''),
        callbacks=templates.get('callbacks', []),
        var_make=templates.get('var_make', ''),
    )

    cpp_templates = cpp_templates or {}
    cls.cpp_templates = MakoTemplates(
        includes=cpp_templates.get('includes', []),
        make=cpp_templates.get('make', ''),
        callbacks=cpp_templates.get('callbacks', []),
        var_make=cpp_templates.get('var_make', ''),
        link=cpp_templates.get('link', []),
        packages=cpp_templates.get('packages', []),
        translations=cpp_templates.get('translations', []),
        declarations=cpp_templates.get('declarations', ''),
    )
    # todo: MakoTemplates.compile() to check for errors

    cls.value = _single_mako_expr(value, block_id)

    return cls


def build_ports(ports_raw, direction):
    ports = []
    port_ids = set()
    stream_port_ids = itertools.count()

    for i, port_params in enumerate(ports_raw):
        port = port_params.copy()
        port['direction'] = direction

        port_id = port.setdefault('id', str(next(stream_port_ids)))
        if port_id in port_ids:
            raise Exception(
                'Port id "{}" already exists in {}s'.format(port_id, direction))
        port_ids.add(port_id)

        ports.append(port)
    return ports


def build_params(params_raw, have_inputs, have_outputs, flags, block_id):
    params = []

    def add_param(**data):
        params.append(data)

    if flags.SHOW_ID in flags:
        add_param(id='id', name='ID', dtype='id', hide='none')
    else:
        add_param(id='id', name='ID', dtype='id', hide='all')

    if not flags.not_dsp:
        add_param(id='alias', name='Block Alias', dtype='string',
                  hide='part', category=ADVANCED_PARAM_TAB)

        if have_outputs or have_inputs:
            add_param(id='affinity', name='Core Affinity', dtype='int_vector',
                      hide='part', category=ADVANCED_PARAM_TAB)

        if have_outputs:
            add_param(id='minoutbuf', name='Min Output Buffer', dtype='int',
                      hide='part', default='0', category=ADVANCED_PARAM_TAB)
            add_param(id='maxoutbuf', name='Max Output Buffer', dtype='int',
                      hide='part', default='0', category=ADVANCED_PARAM_TAB)

    base_params_n = {}
    for param_data in params_raw:
        param_id = param_data['id']
        if param_id in params:
            raise Exception('Param id "{}" is not unique'.format(param_id))

        base_key = param_data.get('base_key', None)
        param_data_ext = base_params_n.get(base_key, {}).copy()
        param_data_ext.update(param_data)

        if 'option_attributes' in param_data:
            _validate_option_attributes(param_data_ext, block_id)

        add_param(**param_data_ext)
        base_params_n[param_id] = param_data_ext

    add_param(id='comment', name='Comment', dtype='_multiline', hide='part',
              default='', category=ADVANCED_PARAM_TAB)
    return params


def _single_mako_expr(value, block_id):
    if not value:
        return None
    value = value.strip()
    if not (value.startswith('${') and value.endswith('}')):
        raise ValueError(
            '{} is not a mako substitution in {}'.format(value, block_id))
    return value[2:-1].strip()


def _validate_option_attributes(param_data, block_id):
    if param_data['dtype'] != 'enum':
        send_warning(
            '{} - option_attributes are for enums only, ignoring'.format(block_id))
        del param_data['option_attributes']
    else:
        for key in list(param_data['option_attributes'].keys()):
            if key in dir(str):
                del param_data['option_attributes'][key]
                send_warning(
                    '{} - option_attribute "{}" overrides str, ignoring'.format(block_id, key))
