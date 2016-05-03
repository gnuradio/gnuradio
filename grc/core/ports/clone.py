# Copyright 2016 Free Software Foundation, Inc.
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

from .port import Port, Element


class PortClone(Port):

    def __init__(self, parent, direction, master, name, key):
        Element.__init__(self, parent)
        self.master_port = master

        self.name = name
        self.key = key
        self.multiplicity = 1

    def __getattr__(self, item):
        return getattr(self.master_port, item)

    def add_clone(self):
        raise NotImplementedError()

    def remove_clone(self, port):
        raise NotImplementedError()
