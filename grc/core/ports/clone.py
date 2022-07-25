# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

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
