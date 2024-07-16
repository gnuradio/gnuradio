"""
Copyright 2008-2015 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""

import collections

from .base import Element
from .Constants import ALIASES_OF
from .utils.descriptors import lazy_property


class Connection(Element):
    """
    Stores information about a connection between two block ports. This class
    knows:
    - Where the source and sink ports are (on which blocks)
    - The domain (message, stream, ...)
    - Which parameters are associated with this connection
    """

    is_connection = True
    category = []
    documentation = {'': ''}
    doc_url = ''

    def __init__(self, parent, source, sink):
        """
        Make a new connection given the parent and 2 ports.

        Args:
            parent: the parent of this element (a flow graph)
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

        # Unlike the blocks, connection parameters are defined in the connection
        # domain definition files, as all connections within the same domain
        # share the same properties.
        param_factory = self.parent_platform.make_param
        conn_parameters = self.parent_platform.connection_params.get(self.type, {})
        self.params = collections.OrderedDict(
            (data['id'], param_factory(parent=self, **data))
            for data in conn_parameters
        )

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

    def children(self):
        """ This includes the connection parameters """
        return self.params.values()

    @lazy_property
    def source_block(self):
        return self.source_port.parent_block

    @lazy_property
    def sink_block(self):
        return self.sink_port.parent_block

    @property
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

    @property
    def label(self):
        """ Returns a label for dialogs """
        src_domain, sink_domain = [
            self.parent_platform.domains[d].name for d in self.type]
        return f'Connection ({src_domain} → {sink_domain})'

    @property
    def namespace_templates(self):
        """Returns everything we want to have available in the template rendering"""
        return {key: param.template_arg for key, param in self.params.items()}

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
            A tuple with connection info, and parameters.
        """
        # See if we need to use file format version 2:
        if self.params:
            return {
                'src_blk_id': self.source_block.name,
                'src_port_id': self.source_port.key,
                'snk_blk_id': self.sink_block.name,
                'snk_port_id': self.sink_port.key,
                'params': collections.OrderedDict(sorted(
                    (param_id, param.value)
                    for param_id, param in self.params.items())),
            }

        # If there's no reason to do otherwise, we can export info as
        # FLOW_GRAPH_FILE_FORMAT_VERSION 1 format:
        return [
            self.source_block.name, self.source_port.key,
            self.sink_block.name, self.sink_port.key,
        ]

    def import_data(self, params):
        """
        Import connection parameters.
        """
        for key, value in params.items():
            try:
                self.params[key].set_value(value)
            except KeyError:
                continue
