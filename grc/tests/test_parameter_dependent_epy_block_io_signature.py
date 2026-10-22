# Copyright 2024 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import pytest

from os import path
import tempfile

from grc.core.platform import Platform


def test_parameter_dependent_epy_block_io_signature():
    grc_file = path.join(path.dirname(__file__),
                         'resources', 'test_parameter_dependent_epy_block_io_signature.grc')
    out_dir = tempfile.gettempdir()
    block_paths = [
        path.join(path.dirname(__file__), '../../grc/blocks'),
        path.join(path.dirname(__file__), '../../gr-blocks/grc')
    ]

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library(block_paths)

    flow_graph = platform.make_flow_graph(grc_file)
    flow_graph.rewrite()
    flow_graph.validate()

    assert flow_graph.is_valid()

    generator = platform.Generator(
        flow_graph, path.join(path.dirname(__file__), 'resources'))
    generator.write()
