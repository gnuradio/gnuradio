# Copyright 2008, 2009, 2015, 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

import weakref

from .utils.descriptors import lazy_property


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
        for child in self.children():
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
        for child in self.children():
            if not child.enabled or child.get_bypassed():
                continue
            for element_msg in child.iter_error_messages():
                yield element_msg

    def rewrite(self):
        """
        Rewrite this element and call rewrite on all children.
        Call this base method before rewriting the element.
        """
        del self._error_messages[:]
        for child in self.children():
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
        from .platform import Platform
        return self.get_parent_by_type(Platform)

    @lazy_property
    def parent_flowgraph(self):
        from .FlowGraph import FlowGraph
        return self.get_parent_by_type(FlowGraph)

    @lazy_property
    def parent_block(self):
        from .blocks import Block
        return self.get_parent_by_type(Block)

    def reset_parents_by_type(self):
        """Reset all lazy properties"""
        for name, obj in vars(Element):  # explicitly only in Element, not subclasses
            if isinstance(obj, lazy_property):
                delattr(self, name)

    def children(self):
        return
        yield  # empty generator

    ##############################################
    # Type testing
    ##############################################
    is_flow_graph = False
    is_block = False
    is_dummy_block = False
    is_connection = False
    is_port = False
    is_param = False
    is_variable = False
    is_import = False
    is_snippet = False

    def get_raw(self, name):
        descriptor = getattr(self.__class__, name, None)
        if not descriptor:
            raise ValueError("No evaluated property '{}' found".format(name))
        return getattr(self, descriptor.name_raw, None) or getattr(self, descriptor.name, None)

    def set_evaluated(self, name, value):
        descriptor = getattr(self.__class__, name, None)
        if not descriptor:
            raise ValueError("No evaluated property '{}' found".format(name))
        self.__dict__[descriptor.name] = value
