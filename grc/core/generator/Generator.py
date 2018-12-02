# Copyright 2008-2016 Free Software Foundation, Inc.
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


from __future__ import absolute_import

import os

from mako.template import Template

from .hier_block import HierBlockGenerator, QtHierBlockGenerator
from .top_block import TopBlockGenerator
from .cpp_top_block import CppTopBlockGenerator
from .cpp_hier_block import CppHierBlockGenerator

DATA_DIR = os.path.dirname(__file__)
FLOW_GRAPH_TEMPLATE = os.path.join(DATA_DIR, 'flow_graph.py.mako')
flow_graph_template = Template(filename=FLOW_GRAPH_TEMPLATE)


class Generator(object):
    """Adaptor for various generators (uses generate_options)"""

    def __init__(self, flow_graph, file_path):
        """
        Initialize the generator object.
        Determine the file to generate.

        Args:
            flow_graph: the flow graph object
            file_path: the path to the grc file
        """
        self.generate_options = flow_graph.get_option('generate_options')
        self.output_language = flow_graph.get_option('output_language')

        if self.output_language == 'python':

            if self.generate_options == 'hb':
                generator_cls = HierBlockGenerator
            elif self.generate_options == 'hb_qt_gui':
                generator_cls = QtHierBlockGenerator
            else:
                generator_cls = TopBlockGenerator

        elif self.output_language == 'cpp':

            if self.generate_options == 'hb':
                generator_cls = CppHierBlockGenerator
            elif self.generate_options == 'hb_qt_gui':
                pass
            else:
                generator_cls = CppTopBlockGenerator

        self._generator = generator_cls(flow_graph, file_path)

    def __getattr__(self, item):
        """get all other attrib from actual generator object"""
        return getattr(self._generator, item)
