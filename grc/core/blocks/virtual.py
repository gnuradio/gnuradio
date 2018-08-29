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

import itertools

from . import Block, register_build_in
from ._build import _build_params, _build_ports

@register_build_in
class VirtualSink(Block):
    count = itertools.count()

    key = 'virtual_sink'
    label = 'Virtual Sink'
    documentation = {'': ''}

    parameters = [dict(
        label='Stream ID',
        id='stream_id',
        dtype='stream_id',
    )]
    inputs = [dict(
        domain='stream',
        dtype=''
    )]
    outputs = []

    def __init__(self, parent, **kwargs):
        self.inputs_data = _build_ports(self.inputs, 'sink') if self.inputs else []
        self.outputs_data = _build_ports(self.outputs, 'source') if self.outputs else []
        self.parameters_data = _build_params(self.parameters or [],
                                bool(self.inputs), bool(self.outputs), self.flags, self.key)

        super(VirtualSink, self).__init__(parent, **kwargs)
        self.params['id'].hide = 'all'

    @property
    def stream_id(self):
        return self.params['stream_id'].value


@register_build_in
class VirtualSource(Block):
    count = itertools.count()

    key = 'virtual_source'
    label = 'Virtual Source'
    documentation = {'': ''}

    parameters = [dict(
        label='Stream ID',
        id='stream_id',
        dtype='stream_id',
    )]
    outputs = [dict(
        domain='stream',
        dtype=''
    )]
    inputs = []

    def __init__(self, parent, **kwargs):
        self.inputs_data = _build_ports(self.inputs, 'sink') if self.inputs else []
        self.outputs_data = _build_ports(self.outputs, 'source') if self.outputs else []
        self.parameters_data = _build_params(self.parameters or [],
                                bool(self.inputs), bool(self.outputs), self.flags, self.key)

        super(VirtualSource, self).__init__(parent, **kwargs)
        self.params['id'].hide = 'all'

    @property
    def stream_id(self):
        return self.params['stream_id'].value
