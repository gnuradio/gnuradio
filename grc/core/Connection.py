"""
Copyright 2008-2015 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from .base import Element
from .Constants import ALIASES_OF
from .utils.descriptors import lazy_property


class Connection(Element):

    is_connection = True

    def __init__(self, parent, source, sink):
        """
        Make a new connection given the parent and 2 ports.

        Args:
            flow_graph: the parent of this element
            source: a port (any direction)
            sink: a port (any direction)
        @throws Error cannot make connection

        Returns:
            a new connection
        """
        Element.__init__(self, parent)

        if not source.is_source:
            source, sink = sink, source
        if not source.is_source:
            raise ValueError('Connection could not isolate source')
        if not sink.is_sink:
            raise ValueError('Connection could not isolate sink')

        self.source_port = source
        self.sink_port = sink

    def __str__(self):
        return 'Connection (\n\t{}\n\t\t{}\n\t{}\n\t\t{}\n)'.format(
            self.source_block, self.source_port, self.sink_block, self.sink_port,
        )

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return NotImplemented
        return self.source_port == other.source_port and self.sink_port == other.sink_port

    def __hash__(self):
        return hash((self.source_port, self.sink_port))

    def __iter__(self):
        return iter((self.source_port, self.sink_port))

    @lazy_property
    def source_block(self):
        return self.source_port.parent_block

    @lazy_property
    def sink_block(self):
        return self.sink_port.parent_block

    @lazy_property
    def type(self):
        return self.source_port.domain, self.sink_port.domain

    @property
    def enabled(self):
        """
        Get the enabled state of this connection.

        Returns:
            true if source and sink blocks are enabled
        """
        return self.source_block.enabled and self.sink_block.enabled

    def validate(self):
        """
        Validate the connections.
        The ports must match in io size.
        """
        Element.validate(self)
        platform = self.parent_platform

        if self.type not in platform.connection_templates:
            self.add_error_message('No connection known between domains "{}" and "{}"'
                                   ''.format(*self.type))

        source_dtype = self.source_port.dtype
        sink_dtype = self.sink_port.dtype
        if source_dtype != sink_dtype and source_dtype not in ALIASES_OF.get(
            sink_dtype, set()
        ):
            self.add_error_message('Source IO type "{}" does not match sink IO type "{}".'.format(
                source_dtype, sink_dtype))

        source_size = self.source_port.item_size
        sink_size = self.sink_port.item_size
        if source_size != sink_size:
            self.add_error_message(
                'Source IO size "{}" does not match sink IO size "{}".'.format(source_size, sink_size))

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this connection's info.

        Returns:
            a nested data odict
        """
        return (
            self.source_block.name, self.source_port.key,
            self.sink_block.name, self.sink_port.key
        )
