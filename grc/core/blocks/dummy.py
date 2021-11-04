# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from . import Block, register_build_in

from ._build import build_params


@register_build_in
class DummyBlock(Block):

    is_dummy_block = True

    label = 'Missing Block'
    key = '_dummy'

    def __init__(self, parent, missing_block_id, parameters, **_):
        self.key = missing_block_id
        self.parameters_data = build_params(
            [], False, False, self.flags, self.key)
        super(DummyBlock, self).__init__(parent=parent)

        param_factory = self.parent_platform.make_param
        for param_id in parameters:
            self.params.setdefault(param_id, param_factory(
                parent=self, id=param_id, dtype='string'))

    def is_valid(self):
        return False

    @property
    def enabled(self):
        return False

    def add_missing_port(self, port_id, direction):
        port = self.parent_platform.make_port(
            parent=self, direction=direction, id=port_id, name='?', dtype='',
        )
        if port.is_source:
            self.sources.append(port)
        else:
            self.sinks.append(port)
        return port
