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

from .Constants import DEFAULT_DOMAIN, GR_STREAM_DOMAIN, GR_MESSAGE_DOMAIN
from .Element import Element

from . import Constants


def _get_source_from_virtual_sink_port(vsp):
    """
    Resolve the source port that is connected to the given virtual sink port.
    Use the get source from virtual source to recursively resolve subsequent ports.
    """
    try:
        return _get_source_from_virtual_source_port(
            vsp.get_enabled_connections()[0].get_source())
    except:
        raise Exception('Could not resolve source for virtual sink port {}'.format(vsp))


def _get_source_from_virtual_source_port(vsp, traversed=[]):
    """
    Recursively resolve source ports over the virtual connections.
    Keep track of traversed sources to avoid recursive loops.
    """
    if not vsp.get_parent().is_virtual_source():
        return vsp
    if vsp in traversed:
        raise Exception('Loop found when resolving virtual source {}'.format(vsp))
    try:
        return _get_source_from_virtual_source_port(
            _get_source_from_virtual_sink_port(
                filter(  # Get all virtual sinks with a matching stream id
                    lambda vs: vs.get_param('stream_id').get_value() == vsp.get_parent().get_param('stream_id').get_value(),
                    filter(  # Get all enabled blocks that are also virtual sinks
                        lambda b: b.is_virtual_sink(),
                        vsp.get_parent().get_parent().get_enabled_blocks(),
                    ),
                )[0].get_sinks()[0]
            ), traversed + [vsp],
        )
    except:
        raise Exception('Could not resolve source for virtual source port {}'.format(vsp))


def _get_sink_from_virtual_source_port(vsp):
    """
    Resolve the sink port that is connected to the given virtual source port.
    Use the get sink from virtual sink to recursively resolve subsequent ports.
    """
    try:
        # Could have many connections, but use first
        return _get_sink_from_virtual_sink_port(
            vsp.get_enabled_connections()[0].get_sink())
    except:
        raise Exception('Could not resolve source for virtual source port {}'.format(vsp))


def _get_sink_from_virtual_sink_port(vsp, traversed=[]):
    """
    Recursively resolve sink ports over the virtual connections.
    Keep track of traversed sinks to avoid recursive loops.
    """
    if not vsp.get_parent().is_virtual_sink():
        return vsp
    if vsp in traversed:
        raise Exception('Loop found when resolving virtual sink {}'.format(vsp))
    try:
        return _get_sink_from_virtual_sink_port(
            _get_sink_from_virtual_source_port(
                filter(  # Get all virtual source with a matching stream id
                    lambda vs: vs.get_param('stream_id').get_value() == vsp.get_parent().get_param('stream_id').get_value(),
                    filter(  # Get all enabled blocks that are also virtual sinks
                        lambda b: b.is_virtual_source(),
                        vsp.get_parent().get_parent().get_enabled_blocks(),
                    ),
                )[0].get_sources()[0]
            ), traversed + [vsp],
        )
    except:
        raise Exception('Could not resolve source for virtual sink port {}'.format(vsp))


class Port(Element):

    is_port = True

    def __init__(self, block, n, dir):
        """
        Make a new port from nested data.

        Args:
            block: the parent element
            n: the nested odict
            dir: the direction
        """
        self._n = n
        if n['type'] == 'message':
            n['domain'] = GR_MESSAGE_DOMAIN
        if 'domain' not in n:
            n['domain'] = DEFAULT_DOMAIN
        elif n['domain'] == GR_MESSAGE_DOMAIN:
            n['key'] = n['name']
            n['type'] = 'message'  # For port color
        if n['type'] == 'msg':
            n['key'] = 'msg'
        if not n.find('key'):
            n['key'] = str(next(block.port_counters[dir == 'source']))

        # Build the port
        Element.__init__(self, block)
        # Grab the data
        self._name = n['name']
        self._key = n['key']
        self._type = n['type'] or ''
        self._domain = n['domain']
        self._hide = n.find('hide') or ''
        self._dir = dir
        self._hide_evaluated = False  # Updated on rewrite()

        self._nports = n.find('nports') or ''
        self._vlen = n.find('vlen') or ''
        self._optional = bool(n.find('optional'))
        self._clones = []  # References to cloned ports (for nports > 1)

    def __str__(self):
        if self.is_source:
            return 'Source - {}({})'.format(self.get_name(), self.get_key())
        if self.is_sink:
            return 'Sink - {}({})'.format(self.get_name(), self.get_key())

    def get_types(self):
        return Constants.TYPE_TO_SIZEOF.keys()

    def is_type_empty(self):
        return not self._n['type']

    def validate(self):
        Element.validate(self)
        if self.get_type() not in self.get_types():
            self.add_error_message('Type "{}" is not a possible type.'.format(self.get_type()))
        platform = self.get_parent().get_parent().get_parent()
        if self.get_domain() not in platform.domains:
            self.add_error_message('Domain key "{}" is not registered.'.format(self.get_domain()))
        if not self.get_enabled_connections() and not self.get_optional():
            self.add_error_message('Port is not connected.')
        # Message port logic
        if self.get_type() == 'msg':
            if self.get_nports():
                self.add_error_message('A port of type "msg" cannot have "nports" set.')
            if self.get_vlen() != 1:
                self.add_error_message('A port of type "msg" must have a "vlen" of 1.')

    def rewrite(self):
        """
        Handle the port cloning for virtual blocks.
        """
        if self.is_type_empty():
            try:
                # Clone type and vlen
                source = self.resolve_empty_type()
                self._type = str(source.get_type())
                self._vlen = str(source.get_vlen())
            except:
                # Reset type and vlen
                self._type = ''
                self._vlen = ''

        Element.rewrite(self)
        hide = self.get_parent().resolve_dependencies(self._hide).strip().lower()
        self._hide_evaluated = False if hide in ('false', 'off', '0') else bool(hide)

        # Update domain if was deduced from (dynamic) port type
        type_ = self.get_type()
        if self._domain == GR_STREAM_DOMAIN and type_ == "message":
            self._domain = GR_MESSAGE_DOMAIN
            self._key = self._name
        if self._domain == GR_MESSAGE_DOMAIN and type_ != "message":
            self._domain = GR_STREAM_DOMAIN
            self._key = '0'  # Is rectified in rewrite()

    def resolve_virtual_source(self):
        if self.get_parent().is_virtual_sink():
            return _get_source_from_virtual_sink_port(self)
        if self.get_parent().is_virtual_source():
            return _get_source_from_virtual_source_port(self)

    def resolve_empty_type(self):
        if self.is_sink:
            try:
                src = _get_source_from_virtual_sink_port(self)
                if not src.is_type_empty():
                    return src
            except:
                pass
            sink = _get_sink_from_virtual_sink_port(self)
            if not sink.is_type_empty():
                return sink
        if self.is_source:
            try:
                src = _get_source_from_virtual_source_port(self)
                if not src.is_type_empty():
                    return src
            except:
                pass
            sink = _get_sink_from_virtual_source_port(self)
            if not sink.is_type_empty():
                return sink

    def get_vlen(self):
        """
        Get the vector length.
        If the evaluation of vlen cannot be cast to an integer, return 1.

        Returns:
            the vector length or 1
        """
        vlen = self.get_parent().resolve_dependencies(self._vlen)
        try:
            return int(self.get_parent().get_parent().evaluate(vlen))
        except:
            return 1

    def get_nports(self):
        """
        Get the number of ports.
        If already blank, return a blank
        If the evaluation of nports cannot be cast to a positive integer, return 1.

        Returns:
            the number of ports or 1
        """
        if self._nports == '':
            return ''

        nports = self.get_parent().resolve_dependencies(self._nports)
        try:
            return max(1, int(self.get_parent().get_parent().evaluate(nports)))
        except:
            return 1

    def get_optional(self):
        return bool(self._optional)

    def get_color(self):
        """
        Get the color that represents this port's type.
        Codes differ for ports where the vec length is 1 or greater than 1.

        Returns:
            a hex color code.
        """
        try:
            color = Constants.TYPE_TO_COLOR[self.get_type()]
            vlen = self.get_vlen()
            if vlen == 1:
                return color
            color_val = int(color[1:], 16)
            r = (color_val >> 16) & 0xff
            g = (color_val >> 8) & 0xff
            b = (color_val >> 0) & 0xff
            dark = (0, 0, 30, 50, 70)[min(4, vlen)]
            r = max(r-dark, 0)
            g = max(g-dark, 0)
            b = max(b-dark, 0)
            # TODO: Change this to .format()
            return '#%.2x%.2x%.2x' % (r, g, b)
        except:
            return '#FFFFFF'

    def get_clones(self):
        """
        Get the clones of this master port (nports > 1)

        Returns:
            a list of ports
        """
        return self._clones

    def add_clone(self):
        """
        Create a clone of this (master) port and store a reference in self._clones.

        The new port name (and key for message ports) will have index 1... appended.
        If this is the first clone, this (master) port will get a 0 appended to its name (and key)

        Returns:
            the cloned port
        """
        # Add index to master port name if there are no clones yet
        if not self._clones:
            self._name = self._n['name'] + '0'
            # Also update key for none stream ports
            if not self._key.isdigit():
                self._key = self._name

        # Prepare a copy of the odict for the clone
        n = self._n.copy()
        # Remove nports from the key so the copy cannot be a duplicator
        if 'nports' in n:
            n.pop('nports')
        n['name'] = self._n['name'] + str(len(self._clones) + 1)
        # Dummy value 99999 will be fixed later
        n['key'] = '99999' if self._key.isdigit() else n['name']

        # Clone
        port = self.__class__(self.get_parent(), n, self._dir)
        self._clones.append(port)
        return port

    def remove_clone(self, port):
        """
        Remove a cloned port (from the list of clones only)
        Remove the index 0 of the master port name (and key9 if there are no more clones left
        """
        self._clones.remove(port)
        # Remove index from master port name if there are no more clones
        if not self._clones:
            self._name = self._n['name']
            # Also update key for none stream ports
            if not self._key.isdigit():
                self._key = self._name

    def get_name(self):
        number = ''
        if self.get_type() == 'bus':
            busses = filter(lambda a: a._dir == self._dir, self.get_parent().get_ports_gui())
            number = str(busses.index(self)) + '#' + str(len(self.get_associated_ports()))
        return self._name + number

    def get_key(self):
        return self._key

    @property
    def is_sink(self):
        return self._dir == 'sink'

    @property
    def is_source(self):
        return self._dir == 'source'

    def get_type(self):
        return self.get_parent().resolve_dependencies(self._type)

    def get_domain(self):
        return self._domain

    def get_hide(self):
        return self._hide_evaluated

    def get_connections(self):
        """
        Get all connections that use this port.

        Returns:
            a list of connection objects
        """
        connections = self.get_parent().get_parent().connections
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
            return [self]
        else:
            if self.is_source:
                get_ports = self.get_parent().get_sources
                bus_structure = self.get_parent().current_bus_structure['source']
            else:
                get_ports = self.get_parent().get_sinks
                bus_structure = self.get_parent().current_bus_structure['sink']

            ports = [i for i in get_ports() if not i.get_type() == 'bus']
            if bus_structure:
                busses = [i for i in get_ports() if i.get_type() == 'bus']
                bus_index = busses.index(self)
                ports = filter(lambda a: ports.index(a) in bus_structure[bus_index], ports)
            return ports
