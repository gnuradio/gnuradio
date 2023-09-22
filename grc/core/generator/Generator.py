# Copyright 2008-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from .hier_block import HierBlockGenerator, QtHierBlockGenerator
from .top_block import TopBlockGenerator
from .cpp_top_block import CppTopBlockGenerator
from .cpp_hier_block import CppHierBlockGenerator


class Generator(object):
    """Adaptor for various generators (uses generate_options)"""

    def __init__(self, flow_graph, output_dir):
        """
        Initialize the generator object.
        Determine the file to generate.

        Args:
            flow_graph: the flow graph object
            output_dir: the output path for generated files
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

        self._generator = generator_cls(flow_graph, output_dir)

    def __getattr__(self, item):
        """get all other attrib from actual generator object"""
        return getattr(self._generator, item)
