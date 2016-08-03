# Copyright 2008, 2009, 2015, 2016 Free Software Foundation, Inc.
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

import weakref
import functools


class lazy_property(object):

    def __init__(self, func):
        self.func = func
        functools.update_wrapper(self, func)

    def __get__(self, instance, cls):
        if instance is None:
            return self
        value = self.func(instance)
        setattr(instance, self.func.__name__, value)
        return value


def nop_write(prop):
    """Make this a property with a nop setter"""
    def nop(self, value):
        pass
    return prop.setter(nop)


class Element(object):

    def __init__(self, parent=None):
        self._parent = weakref.ref(parent) if parent else lambda: None
        self._error_messages = []

    ##################################################
    # Element Validation API
    ##################################################
    def validate(self):
        """
        Validate this element and call validate on all children.
        Call this base method before adding error messages in the subclass.
        """
        del self._error_messages[:]

        for child in self.get_children():
            child.validate()

    def is_valid(self):
        """
        Is this element valid?

        Returns:
            true when the element is enabled and has no error messages or is bypassed
        """
        if not self.enabled or self.get_bypassed():
            return True
        return not next(self.iter_error_messages(), False)

    def add_error_message(self, msg):
        """
        Add an error message to the list of errors.

        Args:
            msg: the error message string
        """
        self._error_messages.append(msg)

    def get_error_messages(self):
        """
        Get the list of error messages from this element and all of its children.
        Do not include the error messages from disabled or bypassed children.
        Cleverly indent the children error messages for printing purposes.

        Returns:
            a list of error message strings
        """
        return [msg if elem is self else "{}:\n\t{}".format(elem, msg.replace("\n", "\n\t"))
                for elem, msg in self.iter_error_messages()]

    def iter_error_messages(self):
        """
        Iterate over error messages. Yields tuples of (element, message)
        """
        for msg in self._error_messages:
            yield self, msg
        for child in self.get_children():
            if not child.enabled or child.get_bypassed():
                continue
            for element_msg in child.iter_error_messages():
                yield element_msg

    def rewrite(self):
        """
        Rewrite this element and call rewrite on all children.
        Call this base method before rewriting the element.
        """
        for child in self.get_children():
            child.rewrite()

    @property
    def enabled(self):
        return True

    def get_bypassed(self):
        return False

    ##############################################
    # Tree-like API
    ##############################################
    @property
    def parent(self):
        return self._parent()

    def get_parent_by_type(self, cls):
        parent = self.parent
        if parent is None:
            return None
        elif isinstance(parent, cls):
            return parent
        else:
            return parent.get_parent_by_type(cls)

    @lazy_property
    def parent_platform(self):
        from .Platform import Platform
        return self.get_parent_by_type(Platform)

    @lazy_property
    def parent_flowgraph(self):
        from .FlowGraph import FlowGraph
        return self.get_parent_by_type(FlowGraph)

    @lazy_property
    def parent_block(self):
        from .Block import Block
        return self.get_parent_by_type(Block)

    def reset_parents_by_type(self):
        """Reset all lazy properties"""
        for name, obj in vars(Element):  # explicitly only in Element, not subclasses
            if isinstance(obj, lazy_property):
                delattr(self, name)

    def get_children(self):
        return list()

    ##############################################
    # Type testing
    ##############################################
    is_platform = False

    is_flow_graph = False

    is_block = False

    is_dummy_block = False

    is_connection = False

    is_port = False

    is_param = False

    is_variable = False

    is_import = False
