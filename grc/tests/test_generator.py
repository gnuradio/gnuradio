# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import logging
import platform as sys_platform
from os import path

from grc.core.platform import Platform


def test_generator():
    """Verify flow graphs, then generate source codes."""
    log = logging.getLogger('test_generator')

    grc_files = [
        path.join(path.dirname(__file__), 'resources', 'test_compiler.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_bokeh_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_python_qt_gui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_nogui_workflow.grc'),
        path.join(path.dirname(__file__), 'resources', 'test_cpp_qt_gui_workflow.grc'),
    ]
    # TODO: There's an issue with setting environment variables in Windows
    # in GrTest.cmake. The hier-block flow graphs require the environment
    # variable GRC_HIER_PATH to be correctly set, but there's a bug on Windows
    # systems that doesn't do that properly. This is a workaround to skip the
    # hier-block flow graphs on Windows systems. It should be removed once the
    # issue is fixed.
    if sys_platform.system() != 'Windows':
        grc_files += [
            path.join(path.dirname(__file__), 'resources', 'test_python_hb_nogui_workflow.grc'),
            path.join(path.dirname(__file__), 'resources', 'test_python_hb_qt_gui_workflow.grc'),
            path.join(path.dirname(__file__), 'resources', 'test_cpp_hb_nogui_workflow.grc'),
            path.join(path.dirname(__file__), 'resources', 'test_cpp_hb_qt_gui_workflow.grc'),
        ]

    # Note: Even if gr-analog and gr-qtgui are disabled, we can read the GRC
    # files. No code will be executed in this test.
    block_paths = [
        path.normpath(path.join(path.dirname(__file__), '../../grc/blocks')),
        path.normpath(path.join(path.dirname(__file__), '../../gr-blocks/grc')),
        path.normpath(path.join(path.dirname(__file__), '../../gr-analog/grc')),
        path.normpath(path.join(path.dirname(__file__), '../../gr-qtgui/grc')),
    ]

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    log.info("Building library...")
    platform.build_library(block_paths)
    log.info("Available workflows:")
    for workflow in platform.workflow_manager.workflows:
        log.info(f"{workflow.id}")

    for grc_file in grc_files:
        log.info(f"Testing {grc_file}...")
        log.info("Loading flow graph...")
        flow_graph = platform.make_flow_graph(grc_file)
        flow_graph.rewrite()
        flow_graph.validate()

        assert flow_graph.is_valid(), \
            f"Flow graph {grc_file} is not valid: " + str(flow_graph.get_error_messages())

        log.info("Calling generator...")
        generator = platform.Generator(
            flow_graph, path.join(path.dirname(__file__), 'resources'))
        generator.write()
