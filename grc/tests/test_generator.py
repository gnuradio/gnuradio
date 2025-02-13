# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import pytest

from os import path
import tempfile

from grc.core.platform import Platform


def test_generator():
    """
    Verify flow graphs then generate source codes
    """
    grc_files = [
        path.join(path.dirname(__file__), 'resources', 'test_compiler.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_bokeh_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_hb_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_hb_qt_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_qt_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_hb_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_hb_qt_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_qt_gui_workflow.grc'),
    ]

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library()

    for grc_file in grc_files:
        print(f"test_generator: Testing {grc_file}...")
        print("Loading flow graph...")
        flow_graph = platform.make_flow_graph(grc_file)
        flow_graph.rewrite()
        flow_graph.validate()

        assert flow_graph.is_valid()

        print("Calling generator...")
        generator = platform.Generator(
            flow_graph, path.join(path.dirname(__file__), 'resources'))
        generator.write()
