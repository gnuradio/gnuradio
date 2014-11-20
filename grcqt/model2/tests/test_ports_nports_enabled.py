"""
Copyright 2014 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import pytest

from .. import FlowGraph
from .. blocks import Block


@pytest.fixture
def block():
    fg = FlowGraph()
    b = Block(fg)
    b.add_stream_sink("in")
    b.add_stream_sink("in2", dtype="int", nports=3)
    b.add_stream_sink("in3")
    b.rewrite()
    return b


def test_nports(block):
    """test the block fixture setup"""
    assert [sink.name for sink in block.sinks] == \
           ["in", "in20", "in21", "in22", "in3"]
    assert [sink.dtype for sink in block.sinks] == \
           [None, "int", "int", "int", None]


def test_reduce_size(block):
    block.sinks[1].nports = 1
    block.rewrite()
    assert [sink.name for sink in block.sinks] == ["in", "in20", "in3"]


def test_increase_size(block):
    block.sinks[1].nports = 5
    block.rewrite()
    assert [sink.name for sink in block.sinks] == \
           ["in", "in20", "in21", "in22", "in23", "in24", "in3"]


def test_enabled(block):
    block._sinks[0].enabled = False
    block.rewrite()
    assert [sink.name for sink in block.sinks] == \
           ["in20", "in21", "in22", "in3"]


def test_enabled2(block):
    block._sinks[1].enabled = False
    block.rewrite()
    assert [sink.name for sink in block.sinks] == \
           ["in", "in3"]
    block._sinks[1].enabled = True
    block.rewrite()
