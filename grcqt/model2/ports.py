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

from abc import ABCMeta, abstractmethod
from itertools import islice

from . base import Element, BlockChildElement


class BasePort(BlockChildElement):
    """Common elements of stream and message ports"""
    __metaclass__ = ABCMeta

    @abstractmethod
    def __init__(self, parent, name, enabled=True, nports=None):
        super(BasePort, self).__init__(parent)
        self._name = name
        self.enabled = enabled
        self.nports = nports

    @property
    def name(self):
        """The name of this block"""
        name = self._name
        if self.nports is not None:
            name += '0'
        return name

    @name.setter
    def name(self, name):
        self._name = name

    @property
    def connections(self):
        """Iterator for the connections using this port"""
        for connection in self.parent_flowgraph.connections:
            if self in connection.ports:
                yield connection

    @property
    def clones(self):
        """Iterator for port clones of this (master) port"""
        for child in self.children:
            if isinstance(child, PortClone):
                yield child

    def disconnect(self):
        """remove all connections to/from this port"""
        for connection in self.connections:
            self.parent_flowgraph.remove(connection)

    def rewrite(self):
        """adjust the number of clones"""
        if self.nports is not None:
            nports = max((self.nports, 1))
            clones = list(self.clones)
            # remove excess clones
            for clone in islice(clones, nports - 1, 1000000):
                clone.disconnect()
                self.children.remove(clone)
            # add new clones
            for i in range(len(clones), nports-1):
                PortClone(master_port=self, clone_id=i+1)  # ref kept in self.children


class PortClone(Element):
    """Acts as a clone of its parent object, but adds and index to its name"""

    def __init__(self, master_port, clone_id):
        super(PortClone, self).__init__(parent=master_port)
        self.clone_id = clone_id

    @property
    def name(self):
        """The name of a cloned port gets its index appended"""
        return self.parent.name[:-1] + str(self.clone_id)

    @property
    def key(self):
        """The key of a cloned port gets its index appended"""
        if isinstance(self.parent, MessagePort):
             return self.parent.key + str(self.clone_id)

    def __getattr__(self, item):
        """Get all other attributes from parent (Port) object"""
        if item != 'clones':
            return getattr(self.parent, item)


BasePort.register(PortClone)


class StreamPort(BasePort):
    """Stream ports have a data type and vector length"""

    def __init__(self, parent, name, enabled=True, nports=None, dtype=None, vlen=1):
        super(StreamPort, self).__init__(parent, name, enabled, nports)
        self.dtype = dtype
        self.vlen = vlen

    def validate(self):
        # todo: check dtype and vlen
        super(StreamPort, self).validate()


class MessagePort(BasePort):
    """Message ports usually have a fixed key"""

    def __init__(self, parent, name, enabled=True, nports=1, key=None):
        super(MessagePort, self).__init__(parent, name, enabled, nports)
        self.key = key or name
