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

from collections import MutableMapping
from itertools import chain

from . import exceptions
from . base import Element
from . import Block, Connection, Variable


class FlowGraph(Element):

    def __init__(self, parent=None):
        super(FlowGraph, self).__init__(parent)

        self.blocks = []
        self.connections = []
        self.variables = {}

        self.options = None
        self.namespace = _FlowGraphNamespace(self.variables)

    def add_variable(self, name, default=None):
        self.variables[name] = Variable(self, name, default)

    def add_block(self, key_or_block):
        """Add a new block to the flow-graph

        Args:
            key: the blocks key (a Block object can be passed as well)

        Raises:
            BlockException
        """
        if isinstance(key_or_block, str):
            try:
                block = self.platform.blocks[key_or_block](parent=self)
            except KeyError:
                raise exceptions.BlockException("Failed to add block '{}'".format(key_or_block))
        elif isinstance(key_or_block, Block):
            block = key_or_block
        else:
            raise exceptions.BlockException("")
        self.blocks.append(block)
        return block

    def make_connection(self, endpoint_a, endpoint_b):
        """Add a connection between the ports of two blocks"""
        connection = Connection(self, endpoint_a, endpoint_b)
        self.connections.append(connection)
        return connection

    def remove(self, elements):
        for element in elements:
            if isinstance(element, Block):
                # todo: remove connections to this block?
                self.blocks.remove(element)
            elif isinstance(element, Connection):
                self.connections.remove(element)
            self.children.remove(element)
            del element

    def rewrite(self):
        self.namespace.reset()
        # todo: decide if lazy var eval is better (if yes, skip and remove finalize
        for name, variable in self.variables.iteritems():
            if name not in self.namespace:
                self.namespace[name] = variable.evaluate()
        self.namespace.finalize()
        # eval blocks first, then connections
        for element in chain(self.blocks, self.connections):
            element.rewrite()

    def evaluate(self, expr):
        """Evaluate an expr in the flow-graph namespace"""
        return eval(str(expr), None, self.namespace)


class _FlowGraphNamespace(MutableMapping):
    """A dict class that auto-calls variables for missing names"""

    def __init__(self, variables, defaults=None):

        self.variables = variables
        self.defaults = defaults if defaults else {}

        self._namespace = dict(self.defaults)
        self._dependency_chain = []
        self._finalized = False

    def __getitem__(self, key):
        if key in self._dependency_chain:
            raise RuntimeError("Circular dependency")

        if not self._finalized and key not in self._namespace and key in self.variables:
            self._dependency_chain.append(key)
            self[key] = self.variables[key].evaluate()
            self._dependency_chain.remove(key)

        if self._dependency_chain:
            self.variables[self._dependency_chain[-1]].dependencies.add(key)

        return self._namespace[key]

    def __setitem__(self, key, value):
        if not self._finalized:
            self._namespace[key] = value

    def __delitem__(self, key):
        del self._namespace[key]

    def __len__(self):
        return len(self._namespace)

    def __iter__(self):
        return iter(self._namespace)

    def finalize(self):
        del self._dependency_chain[:]
        self._finalized = True

    def reset(self):
        del self._dependency_chain[:]
        self._namespace.clear()
        self._namespace.update(self.defaults)
        self._finalized = False
