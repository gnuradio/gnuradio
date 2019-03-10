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
from ._build import build_params


@register_build_in
class VirtualSink(Block):
    count = itertools.count()

    key = 'virtual_sink'
    label = 'Virtual Sink'

    parameters_data = build_params(
        params_raw=[dict(label='Stream ID', id='stream_id', dtype='stream_id')],
        have_inputs=False, have_outputs=False, flags=Block.flags, block_id=key
    )
    inputs_data = [dict(domain='stream', dtype='', direction='sink', id="0")]

    def __init__(self, parent, **kwargs):
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

    parameters_data = build_params(
        params_raw=[dict(label='Stream ID', id='stream_id', dtype='stream_id')],
        have_inputs=False, have_outputs=False, flags=Block.flags, block_id=key
    )
    outputs_data = [dict(domain='stream', dtype='', direction='source', id="0")]

    def __init__(self, parent, **kwargs):
        super(VirtualSource, self).__init__(parent, **kwargs)
        self.params['id'].hide = 'all'

    @property
    def stream_id(self):
        return self.params['stream_id'].value
