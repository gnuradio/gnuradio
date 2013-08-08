"""
Copyright 2008-2011 Free Software Foundation, Inc.
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

from Element import Element

class Port(Element):

    def __init__(self, block, n, dir):
        """
        Make a new port from nested data.
        
        Args:
            block: the parent element
            n: the nested odict
            dir: the direction source or sink
        """
        #build the port
        Element.__init__(self, block)
        #grab the data
        self._name = n['name']
        self._key = n['key']
        self._type = n['type']
        self._dir = dir

    def validate(self):
        """
        Validate the port.
        The port must be non-empty and type must a possible type.
        """
        Element.validate(self)
        if self.get_type() not in self.get_types():
            self.add_error_message('Type "%s" is not a possible type.'%self.get_type())

    def __str__(self):
        if self.is_source():
            return 'Source - %s(%s)'%(self.get_name(), self.get_key())
        if self.is_sink():
            return 'Sink - %s(%s)'%(self.get_name(), self.get_key())

    def get_types(self):
        """
        Get a list of all possible port types.
        @throw NotImplementedError
        """
        raise NotImplementedError

    def is_port(self): return True
    def get_color(self): return '#FFFFFF'
    def get_name(self):
        number = ''
        if self.get_type() == 'bus':
            busses = filter(lambda a: a._dir == self._dir, self.get_parent().get_ports_gui());
            
            number = str(busses.index(self)) + '#' + str(len(self.get_associated_ports()));
        return self._name + number

    def get_key(self): return self._key
    def is_sink(self): return self._dir == 'sink'
    def is_source(self): return self._dir == 'source'
    def get_type(self): return self.get_parent().resolve_dependencies(self._type)

    def get_connections(self):
        """
        Get all connections that use this port.
        
        Returns:
            a list of connection objects
        """
        connections = self.get_parent().get_parent().get_connections()
        connections = filter(lambda c: c.get_source() is self or c.get_sink() is self, connections)
        return connections

    def get_enabled_connections(self):
        """
        Get all enabled connections that use this port.
        
        Returns:
            a list of connection objects
        """
        return filter(lambda c: c.get_enabled(), self.get_connections())

    def get_associated_ports(self):
        if not self.get_type() == 'bus':
            return [self];
        else:
            if self.is_source():
                get_p = self.get_parent().get_sources;
                bus_structure = self.get_parent().current_bus_structure['source'];
                direc = 'source'
            else:
                get_p = self.get_parent().get_sinks;
                bus_structure = self.get_parent().current_bus_structure['sink'];
                direc = 'sink'
            
            ports = [i for i in get_p() if not i.get_type() == 'bus'];
            if bus_structure:
                busses = [i for i in get_p() if i.get_type() == 'bus'];
                bus_index = busses.index(self);
                ports = filter(lambda a: ports.index(a) in bus_structure[bus_index], ports);
            return ports;
