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

from . import Constants
from .Element import Element
from .utils import odict


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
        busses = len(filter(lambda a: a.get_type() == 'bus', [source, sink])) % 2
        if not busses == 0:
            raise ValueError('busses must get with busses')

        if not len(source.get_associated_ports()) == len(sink.get_associated_ports()):
            raise ValueError('port connections must have same cardinality')
        # Ensure that this connection (source -> sink) is unique
        for connection in flow_graph.connections:
            if connection.get_source() is source and connection.get_sink() is sink:
                raise LookupError('This connection between source and sink is not unique.')
        self._source = source
        self._sink = sink
        if source.get_type() == 'bus':

            sources = source.get_associated_ports()
            sinks = sink.get_associated_ports()

            for i in range(len(sources)):
                try:
                    flow_graph.connect(sources[i], sinks[i])
                except:
                    pass

    def __str__(self):
        return 'Connection (\n\t{0}\n\t\t{1}\n\t{2}\n\t\t{3}\n)'.format(
            self.get_source().get_parent(),
            self.get_source(),
            self.get_sink().get_parent(),
            self.get_sink(),
        )

    def is_msg(self):
        return self.get_source().get_type() == self.get_sink().get_type() == 'msg'

    def is_bus(self):
        return self.get_source().get_type() == self.get_sink().get_type() == 'bus'

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
        platform = self.get_parent().get_parent()
        source_domain = self.get_source().get_domain()
        sink_domain = self.get_sink().get_domain()
        if (source_domain, sink_domain) not in platform.connection_templates:
            self.add_error_message('No connection known for domains "{0}", "{1}"'.format(
                    source_domain, sink_domain))
        too_many_other_sinks = (
            not platform.domains.get(source_domain, []).get('multiple_sinks', False) and
            len(self.get_source().get_enabled_connections()) > 1
        )
        too_many_other_sources = (
            not platform.domains.get(sink_domain, []).get('multiple_sources', False) and
            len(self.get_sink().get_enabled_connections()) > 1
        )
        if too_many_other_sinks:
            self.add_error_message(
                'Domain "{0}" can have only one downstream block'.format(source_domain))
        if too_many_other_sources:
            self.add_error_message(
                'Domain "{0}" can have only one upstream block'.format(sink_domain))

        source_size = Constants.TYPE_TO_SIZEOF[self.get_source().get_type()] * self.get_source().get_vlen()
        sink_size = Constants.TYPE_TO_SIZEOF[self.get_sink().get_type()] * self.get_sink().get_vlen()
        if source_size != sink_size:
            self.add_error_message('Source IO size "{0}" does not match sink IO size "{1}".'.format(source_size, sink_size))

    def get_enabled(self):
        """
        Get the enabled state of this connection.

        Returns:
            true if source and sink blocks are enabled
        """
        return self.get_source().get_parent().get_enabled() and \
            self.get_sink().get_parent().get_enabled()

    #############################
    # Access Ports
    #############################
    def get_sink(self):
        return self._sink

    def get_source(self):
        return self._source

    ##############################################
    # Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this connection's info.

        Returns:
            a nested data odict
        """
        n = odict()
        n['source_block_id'] = self.get_source().get_parent().get_id()
        n['sink_block_id'] = self.get_sink().get_parent().get_id()
        n['source_key'] = self.get_source().get_key()
        n['sink_key'] = self.get_sink().get_key()
        return n
