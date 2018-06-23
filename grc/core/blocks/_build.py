# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from __future__ import absolute_import

import collections
import itertools
import re

from ..Constants import ADVANCED_PARAM_TAB
from ..utils import to_list

from .block import Block
from ._flags import Flags
from ._templates import MakoTemplates


def build(id, label='', category='', flags='', documentation='',
          value=None, asserts=None,
          parameters=None, inputs=None, outputs=None, templates=None, **kwargs):
    block_id = id

    cls = type(str(block_id), (Block,), {})
    cls.key = block_id

    cls.label = label or block_id.title()
    cls.category = [cat.strip() for cat in category.split('/') if cat.strip()]

    cls.flags = Flags(to_list(flags))
    if re.match(r'options$|variable|virtual', block_id):
        cls.flags.set(Flags.NOT_DSP, Flags.DISABLE_BYPASS)

    cls.documentation = {'': documentation.strip('\n\t ').replace('\\\n', '')}

    cls.asserts = [_single_mako_expr(a, block_id) for a in to_list(asserts)]

    cls.inputs_data = _build_ports(inputs, 'sink') if inputs else []
    cls.outputs_data = _build_ports(outputs, 'source') if outputs else []
    cls.parameters_data = _build_params(parameters or [],
                                        bool(cls.inputs_data), bool(cls.outputs_data), cls.flags)
    cls.extra_data = kwargs

    templates = templates or {}
    cls.templates = MakoTemplates(
        imports=templates.get('imports', ''),
        make=templates.get('make', ''),
        callbacks=templates.get('callbacks', []),
        var_make=templates.get('var_make', ''),
    )
    # todo: MakoTemplates.compile() to check for errors

    cls.value = _single_mako_expr(value, block_id)

    return cls


def _build_ports(ports_raw, direction):
    ports = []
    port_ids = set()
    stream_port_ids = itertools.count()

    for i, port_params in enumerate(ports_raw):
        port = port_params.copy()
        port['direction'] = direction

        port_id = port.setdefault('id', str(next(stream_port_ids)))
        if port_id in port_ids:
            raise Exception('Port id "{}" already exists in {}s'.format(port_id, direction))
        port_ids.add(port_id)

        ports.append(port)
    return ports


def _build_params(params_raw, have_inputs, have_outputs, flags):
    params = []

    def add_param(**data):
        params.append(data)

    add_param(id='id', name='ID', dtype='id', hide='part')

    if not flags.not_dsp:
        add_param(id='alias', name='Block Alias', dtype='string',
                  hide='part', category=ADVANCED_PARAM_TAB)

        if have_outputs or have_inputs:
            add_param(id='affinity', name='Core Affinity', dtype='int_vector',
                      hide='part', category=ADVANCED_PARAM_TAB)

        if have_outputs:
            add_param(id='minoutbuf', name='Min Output Buffer', dtype='int',
                      hide='part', value='0', category=ADVANCED_PARAM_TAB)
            add_param(id='maxoutbuf', name='Max Output Buffer', dtype='int',
                      hide='part', value='0', category=ADVANCED_PARAM_TAB)

    base_params_n = {}
    for param_data in params_raw:
        param_id = param_data['id']
        if param_id in params:
            raise Exception('Param id "{}" is not unique'.format(param_id))

        base_key = param_data.get('base_key', None)
        param_data_ext = base_params_n.get(base_key, {}).copy()
        param_data_ext.update(param_data)

        add_param(**param_data_ext)
        base_params_n[param_id] = param_data_ext

    add_param(id='comment', name='Comment', dtype='_multiline', hide='part',
              value='', category=ADVANCED_PARAM_TAB)
    return params


def _single_mako_expr(value, block_id):
    if not value:
        return None
    value = value.strip()
    if not (value.startswith('${') and value.endswith('}')):
        raise ValueError('{} is not a mako substitution in {}'.format(value, block_id))
    return value[2:-1].strip()
