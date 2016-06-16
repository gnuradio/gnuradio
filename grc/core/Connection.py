"""
Copyright 2008-2015 Free Software Foundation, Inc.
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

from __future__ import absolute_import

import collections

from six.moves import range

from . import Constants
from .Element import Element, lazy_property


class Connection(Element):

    is_connection = True

    def __init__(self, flow_graph, porta, portb):
        """
        Make a new connection given the parent and 2 ports.

        Args:
            flow_graph: the parent of this element
            porta: a port (any direction)
            portb: a port (any direction)
        @throws Error cannot make connection

        Returns:
            a new connection
        """
        Element.__init__(self, flow_graph)

        source, sink = self._get_sink_source(porta, portb)

        self.source_port = source
        self.sink_port = sink

        # Ensure that this connection (source -> sink) is unique
        for connection in flow_graph.connections:
            if connection.source_port is source and connection.sink_port is sink:
                raise LookupError('This connection between source and sink is not unique.')

        self._make_bus_connect()

    @staticmethod
    def _get_sink_source(porta, portb):
        source = sink = None
        # Separate the source and sink
        for port in (porta, portb):
            if port.is_source:
                source = port
            else:
                sink = port
        if not source:
            raise ValueError('Connection could not isolate source')
        if not sink:
            raise ValueError('Connection could not isolate sink')
        return source, sink

    def __str__(self):
        return 'Connection (\n\t{}\n\t\t{}\n\t{}\n\t\t{}\n)'.format(
            self.source_block, self.source_port, self.sink_block, self.sink_port,
        )

    def is_msg(self):
        return self.source_port.get_type() == self.sink_port.get_type() == 'msg'

    def is_bus(self):
        return self.source_port.get_type() == self.sink_port.get_type() == 'bus'

    def validate(self):
        """
        Validate the connections.
        The ports must match in io size.
        """
        """
        Validate the connections.
        The ports must match in type.
        """
        Element.validate(self)
        platform = self.parent_platform

        source_domain = self.source_port.get_domain()
        sink_domain = self.sink_port.get_domain()

        if (source_domain, sink_domain) not in platform.connection_templates:
            self.add_error_message('No connection known for domains "{}", "{}"'.format(
                source_domain, sink_domain))
        too_many_other_sinks = (
            not platform.domains.get(source_domain, []).get('multiple_sinks', False) and
            len(self.source_port.get_enabled_connections()) > 1
        )
        too_many_other_sources = (
            not platform.domains.get(sink_domain, []).get('multiple_sources', False) and
            len(self.sink_port.get_enabled_connections()) > 1
        )
        if too_many_other_sinks:
            self.add_error_message(
                'Domain "{}" can have only one downstream block'.format(source_domain))
        if too_many_other_sources:
            self.add_error_message(
                'Domain "{}" can have only one upstream block'.format(sink_domain))

        source_size = Constants.TYPE_TO_SIZEOF[self.source_port.get_type()] * self.source_port.get_vlen()
        sink_size = Constants.TYPE_TO_SIZEOF[self.sink_port.get_type()] * self.sink_port.get_vlen()
        if source_size != sink_size:
            self.add_error_message('Source IO size "{}" does not match sink IO size "{}".'.format(source_size, sink_size))

    def get_enabled(self):
        """
        Get the enabled state of this connection.

        Returns:
            true if source and sink blocks are enabled
        """
        return self.source_block.get_enabled() and self.sink_block.get_enabled()

    @lazy_property
    def source_block(self):
        return self.source_port.parent_block

    @lazy_property
    def sink_block(self):
        return self.sink_port.parent_block

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this connection's info.

        Returns:
            a nested data odict
        """
        n = collections.OrderedDict()
        n['source_block_id'] = self.source_block.get_id()
        n['sink_block_id'] = self.sink_block.get_id()
        n['source_key'] = self.source_port.key
        n['sink_key'] = self.sink_port.key
        return n

    def _make_bus_connect(self):
        source, sink = self.source_port, self.sink_port

        if (source.get_type() == 'bus') != (sink.get_type() == 'bus'):
            raise ValueError('busses must get with busses')

        if not len(source.get_associated_ports()) == len(sink.get_associated_ports()):
            raise ValueError('port connections must have same cardinality')

        if source.get_type() == 'bus':
            sources = source.get_associated_ports()
            sinks = sink.get_associated_ports()

            for i in range(len(sources)):
                try:
                    self.parent_flowgraph.connect(sources[i], sinks[i])
                except:
                    pass
