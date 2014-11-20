"""
Copyright 2014 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

from __future__ import absolute_import, division, print_function

import re
from abc import ABCMeta
from collections import namedtuple
from functools import partial
from itertools import imap, count

from . import exceptions
from . base import BlockChildElement
from . _consts import BLOCK_ID_BLACK_LIST


class Param(BlockChildElement):
    __metaclass__ = ABCMeta

    def __init__(self, parent, name, key, vtype=None, default=None, category=None, validator=None):
        super(Param, self).__init__(parent)
        self._key = key
        self.name = name
        self.category = category
        self._evaluated = None

        self.vtype = vtype
        self.validator = validator
        self.value = self.default = default  # todo get vtype default

    @property
    def key(self):
        return self._key

    @property
    def evaluated(self):
        return self._evaluated

    def rewrite(self):
        super(Param, self).rewrite()
        self._evaluated = self.parent_flowgraph.evaluate(self.value)

    def validate(self):
        for error in super(Param, self).validate():
            yield error

        if not self.vtype in self.platform.vtypes:
            yield self, "Unknown param value type '{}' "

        try:
            # value type validation
            self.platform.vtypes[self.vtype].validate(self.evaluated)
            # custom validator
            if callable(self.validator) and not self.validator(self.evaluated):
                yield exceptions.ValidationException(
                    self, "Validator failed: " + repr(self.validator)
                )
        except Exception as e:
            yield self, "Failed to validate " + e.args[0]


class IdParam(Param):
    """Parameter of a block used as a unique parameter within a flow-graph"""

    _id_matcher = re.compile('^[a-z|A-Z]\w*$')

    def __init__(self, parent):
        super(IdParam, self).__init__(parent, 'ID', 'id', str)
        self.default = self._get_unique_block_id()

    def _get_unique_block_id(self):
        """get a unique block id within the flow-graph by trail&error"""
        block = self.parent_block
        blocks = self.parent_flowgraph.blocks
        for block_id in imap(lambda key: "{}_{}".format(key, block), count()):
            if block_id not in blocks:
                return block_id

    def validate(self):
        for error in super(IdParam, self).validate():
            yield error

        id_value = self.evaluated
        is_duplicate_id = any(
            block.id == id_value
            for block in self.parent_flowgraph.blocks if block is not self
        )
        if not self._id_matcher.match(id_value):
            yield exceptions.ValidationException(
                self, "Invalid ID"
            )
        elif id_value in BLOCK_ID_BLACK_LIST:
            yield exceptions.ValidationException(
                self, "ID is blacklisted"
            )
        elif is_duplicate_id:
            yield exceptions.ValidationException(
                self, "Duplicate ID"
            )


class OptionsParam(Param):

    # careful: same empty dict for all instances
    Option = partial(namedtuple("Option", "name value extra"), extra={})

    def __init__(self, parent, name, key, vtype, options, default=None, allow_arbitrary_values=False):
        super(OptionsParam, self).__init__(parent, name, key, vtype, default)

        self._options = options
        self.options = []
        self.allow_arbitrary_values = allow_arbitrary_values

    def add_option(self, name_or_option, value, extra=None):
        if isinstance(name_or_option, self.option):
            option = name_or_option
        else:
            option = self.Option(name_or_option, value, extra or {})
        self.options.append(option)

    def rewrite(self):
        del self.options[:]
        for option in self._options:
            if not isinstance(option, self.Option):
                self.options.append(self.Option(*option))

        super(OptionsParam, self).rewrite()

    def validate(self):
        for error in super(OptionsParam, self).validate():
            yield error
        value = self.evaluated
        if not self.allow_arbitrary_values and value not in imap(lambda o: o.value, self.options):
            yield exceptions.ValidationException(
                self, "Value '{}' not allowed".format(value)
            )
