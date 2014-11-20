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

from . import exceptions


class Element(object):

    def __init__(self, parent):
        super(Element, self).__init__()
        self._parent = parent
        self._children = []
        try:
            parent.children.append(self)
        except AttributeError:
            pass

    def get_parent_by_class(self, cls):
        parent = self.parent
        return parent if isinstance(parent, cls) else \
            parent.get_parent_by_class(cls) if parent else None

    @property
    def parent(self):
        """Get the parent object"""
        return self._parent

    @property
    def parent_block(self):
        """Get the first block object in the ancestry

         Returns:
            a block object or None
        """
        from . blocks import BaseBlock
        return self.get_parent_by_class(BaseBlock)

    @property
    def parent_flowgraph(self):
        """Get the first flow-graph object in the ancestry

         Returns:
            a flow-graph object or None
        """
        from . flowgraph import FlowGraph
        return self.get_parent_by_class(FlowGraph)

    @property
    def platform(self):
        """Get the platform object from the ancestry

         Returns:
            a platform object or None
        """
        from . platform import Platform
        return self.get_parent_by_class(Platform)

    @property
    def children(self):
        return self._children

    def rewrite(self):
        """Rewrite object and all child object in this tree

        A rewrite shall be used to reevaluate variables, parameters, block surface (ports,...), ...
        The resulting values shall be cached for fast access
        """
        for child in self.children:
            child.rewrite()

    def validate(self):
        """Validate object and all child object in this tree

        Validation shall only check the validity of the flow-graph, not change any values
        """
        for child in self.children:
            # py3.3: yield from
            for error in child.validate():
                yield error


class BlockChildElement(Element):
    """Adds install rewrite callbacks for specific obt attributes"""

    def __init__(self, parent):
        super(BlockChildElement, self).__init__(parent)
        self.rewrite_actions = {}

    def rewrite(self):
        """Perform a rewrite based on a set of callbacks"""
        super(BlockChildElement, self).rewrite()
        params = self.parent_block.params_namespace
        for target, callback_or_param_name in self.rewrite_actions.iteritems():
            try:
                if callable(callback_or_param_name):
                    value = callback_or_param_name(params)
                else:
                    value = params[callback_or_param_name]

                setattr(self, target, value)

            except Exception as e:
                raise exceptions.BlockException(e)

    def on_rewrite(self, **kwargs):
        """This installs a number of callbacks in the objects rewrite function

        The object must a child of a Block. The key of each argument must be a
        valid attribute of the object. The values are callables with a single
        argument. On rewrite a dict of parameter keys and evaluated value is
        passed.

        As a shorthand a parameter key (string) can be passed instead of a
        callable. The parameter value is used to update the attribute.
        """
        invalid_attr_names = []
        for attr_name, callback in kwargs.iteritems():
            if hasattr(self, attr_name):  #todo: exclude methods
                self.rewrite_actions[attr_name] = callback
            else:
                invalid_attr_names.append(attr_name)
        if invalid_attr_names:
            raise exceptions.BlockSetupException(
                "No port attribute(s) founds for " + str(invalid_attr_names)
            )
