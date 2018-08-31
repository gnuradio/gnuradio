# Copyright 2017 Free Software Foundation, Inc.
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

from os import path
import sys

from grc.converter import flow_graph


def test_flow_graph_converter():
    filename = path.join(path.dirname(__file__), 'resources', 'test_compiler.grc')

    data = flow_graph.from_xml(filename)

    flow_graph.dump(data, sys.stdout)


def test_flow_graph_converter_with_fp():
    filename = path.join(path.dirname(__file__), 'resources', 'test_compiler.grc')

    with open(filename) as fp:
        data = flow_graph.from_xml(fp)

    flow_graph.dump(data, sys.stdout)

