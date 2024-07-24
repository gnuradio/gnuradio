# Copyright 2008-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


import importlib
import sys


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
        self.generator_class_name = flow_graph.get_option('generator_class_name')
        self.generator_module = flow_graph.get_option('generator_module')

        generator_module = importlib.import_module(self.generator_module)
        generator_cls = getattr(generator_module, self.generator_class_name)

        self._generator = generator_cls(flow_graph, output_dir)

    def __getattr__(self, item):
        """get all other attrib from actual generator object"""
        return getattr(self._generator, item)
