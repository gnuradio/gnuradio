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

import re

from .block import Block
from ._flags import Flags
from ._templates import MakoTemplates


def build(id, label='', category='', flags='', documentation='',
          checks=None, value=None,
          parameters=None, inputs=None, outputs=None, templates=None, **kwargs):
    block_id = id

    cls = type(block_id, (Block,), {})
    cls.key = block_id

    cls.label = label or block_id.title()
    cls.category = [cat.strip() for cat in category.split('/') if cat.strip()]

    cls.flags = Flags(flags)
    if re.match(r'options$|variable|virtual', block_id):
        cls.flags += Flags.NOT_DSP + Flags.DISABLE_BYPASS

    cls.documentation = {'': documentation.strip('\n\t ').replace('\\\n', '')}

    cls.checks = [_single_mako_expr(check, block_id) for check in (checks or [])]

    cls.parameters_data = parameters or []
    cls.inputs_data = inputs or []
    cls.outputs_data = outputs or []
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


def _single_mako_expr(value, block_id):
    match = re.match(r'\s*\$\{\s*(.*?)\s*\}\s*', str(value))
    if value and not match:
        raise ValueError('{} is not a mako substitution in {}'.format(value, block_id))
    return match.group(1) if match else None
