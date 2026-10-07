# Copyright 2026 Ettus Research, a National Instruments Brand
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from os import path

import pytest

from gnuradio import gr
from grc.core.platform import Platform
from grc.core.utils.autoconnect import find_connections

BLOCK_WIDTH = 100
PORT_SPACING = 20


@pytest.fixture(scope='module')
def platform():
    block_paths = [
        path.normpath(path.join(path.dirname(__file__), '../../grc/blocks')),
        path.normpath(path.join(path.dirname(__file__), '../../gr-blocks/grc')),
    ]
    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
        install_prefix=gr.prefix(),
    )
    platform.build_library(block_paths)
    return platform


def port_geometry(port):
    """Simplified geometry: block at 'coordinate', rotated by 0 or 180 deg"""
    x, y = port.parent_block.states['coordinate']
    flipped = port.parent_block.states['rotation'] == 180
    ports = port.parent_block.sources if port.is_source else port.parent_block.sinks
    py = y + PORT_SPACING * ports.index(port)
    on_right = port.is_source != flipped
    px = x + BLOCK_WIDTH if on_right else x
    return (px, py), ((1.0 if on_right else -1.0), 0.0)


def add_block(flow_graph, key, coordinate, rotation=0, **params):
    block = flow_graph.new_block(key)
    block.states['coordinate'] = coordinate
    block.states['rotation'] = rotation
    for name, value in params.items():
        block.params[name].set_value(value)
    return block


def update(flow_graph):
    flow_graph.rewrite()
    flow_graph.validate()


def test_chain_left_to_right(platform):
    fg = platform.make_flow_graph()
    # Deliberately created out of order
    snk = add_block(fg, 'blocks_null_sink', (400, 0), type='float')
    src = add_block(fg, 'blocks_null_source', (0, 0), type='float')
    cpy = add_block(fg, 'blocks_copy', (200, 0), type='float')
    update(fg)
    pairs = find_connections([snk, src, cpy], port_geometry)
    assert {(s.parent_block, k.parent_block) for s, k in pairs} == {(src, cpy), (cpy, snk)}


def test_right_to_left_rotated(platform):
    fg = platform.make_flow_graph()
    src = add_block(fg, 'blocks_null_source', (400, 0), rotation=180, type='float')
    snk = add_block(fg, 'blocks_null_sink', (0, 0), rotation=180, type='float')
    update(fg)
    pairs = find_connections([src, snk], port_geometry)
    assert [(s.parent_block, k.parent_block) for s, k in pairs] == [(src, snk)]


def test_no_backwards_connection(platform):
    fg = platform.make_flow_graph()
    # Sink is to the left of the source, both facing right
    snk = add_block(fg, 'blocks_null_sink', (0, 0), type='float')
    src = add_block(fg, 'blocks_null_source', (400, 0), type='float')
    update(fg)
    assert find_connections([src, snk], port_geometry) == []


def test_type_mismatch(platform):
    fg = platform.make_flow_graph()
    src = add_block(fg, 'blocks_null_source', (0, 0), type='float')
    snk = add_block(fg, 'blocks_null_sink', (200, 0), type='complex')
    update(fg)
    assert find_connections([src, snk], port_geometry) == []


def test_multi_port_in_order(platform):
    fg = platform.make_flow_graph()
    src = add_block(fg, 'blocks_null_source', (0, 0), type='float', num_outputs='2')
    # Offset downwards, which would make greedy matching cross the ports
    snk = add_block(fg, 'blocks_null_sink', (200, 50), type='float', num_inputs='2')
    update(fg)
    pairs = find_connections([src, snk], port_geometry)
    assert [(s.key, k.key) for s, k in pairs] == [('0', '0'), ('1', '1')]


def test_fan_out(platform):
    fg = platform.make_flow_graph()
    src = add_block(fg, 'blocks_null_source', (0, 0), type='float')
    snk1 = add_block(fg, 'blocks_null_sink', (200, 0), type='float')
    snk2 = add_block(fg, 'blocks_null_sink', (200, 100), type='float')
    update(fg)
    pairs = find_connections([src, snk1, snk2], port_geometry)
    assert {(s.parent_block, k.parent_block) for s, k in pairs} == {(src, snk1), (src, snk2)}


def test_fan_in_by_position(platform):
    fg = platform.make_flow_graph()
    add = add_block(fg, 'blocks_add_xx', (200, 0), type='float', num_inputs='2')
    src_bottom = add_block(fg, 'blocks_null_source', (0, 40), type='float')
    src_top = add_block(fg, 'blocks_null_source', (0, -20), type='float')
    update(fg)
    pairs = find_connections([add, src_bottom, src_top], port_geometry)
    assert sorted((k.key, s.parent_block.name) for s, k in pairs) == [
        ('0', src_top.name), ('1', src_bottom.name)]


def test_skip_existing_connections(platform):
    fg = platform.make_flow_graph()
    src = add_block(fg, 'blocks_null_source', (0, 0), type='float')
    cpy = add_block(fg, 'blocks_copy', (200, 0), type='float')
    snk = add_block(fg, 'blocks_null_sink', (400, 0), type='float')
    fg.connect(src.sources[0], cpy.sinks[0])
    update(fg)
    pairs = find_connections([src, cpy, snk], port_geometry)
    assert [(s.parent_block, k.parent_block) for s, k in pairs] == [(cpy, snk)]
