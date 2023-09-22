# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import itertools

from . import Block, register_build_in
from ._build import build_params


@register_build_in
class VirtualSink(Block):
    count = itertools.count()

    key = 'virtual_sink'
    label = 'Virtual Sink'
    flags = Block.flags
    flags.set('cpp')

    parameters_data = build_params(
        params_raw=[
            dict(label='Stream ID', id='stream_id', dtype='stream_id')],
        have_inputs=False, have_outputs=False, flags=flags, block_id=key
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
    flags = Block.flags
    flags.set('cpp')

    parameters_data = build_params(
        params_raw=[
            dict(label='Stream ID', id='stream_id', dtype='stream_id')],
        have_inputs=False, have_outputs=False, flags=flags, block_id=key
    )
    outputs_data = [dict(domain='stream', dtype='',
                         direction='source', id="0")]

    def __init__(self, parent, **kwargs):
        super(VirtualSource, self).__init__(parent, **kwargs)
        self.params['id'].hide = 'all'

    @property
    def stream_id(self):
        return self.params['stream_id'].value
