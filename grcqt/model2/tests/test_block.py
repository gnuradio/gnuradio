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

from __future__ import absolute_import, division, print_function
import pytest

from .. flowgraph import FlowGraph
from .. blocks import Block
from .. ports import StreamPort
from .. params import Param
from .. import exceptions


@pytest.fixture
def block():
    return Block(FlowGraph())


def test_block_add_param1(block):
    # add an instance
    block.add_param(Param(block, "test name", "t1", vtype=int))
    assert "t1" in block.params
    assert block.params["t1"].name == "test name"
    assert block.params["t1"].vtype == int


def test_block_add_param2(block):
    # add with custom class
    class MyParam(Param): pass
    block.add_param("t", "t2", cls=MyParam)
    assert "t2" in block.params


def test_block_add_param3(block):
    # add with custom class through ABC register
    class MyParam2(object):
        def __init__(self, parent, name, key):
            self.key = key
    Param.register(MyParam2)

    block.add_param("t", "t2", cls=MyParam2)
    assert "t2" in block.params


def test_block_add_param4(block):
    # add a normal param
    p = block.add_param("My test", "t3", default="test")
    assert "t3" in block.params
    assert isinstance(p, Param)
    assert p.vtype is None


def test_block_add_param5(block):
    try:
        class NotAParamSubclass(object): pass
        block.add_param("t", "t4", cls=NotAParamSubclass)
    except exceptions.BlockSetupException:
        pass
    else:
        assert False


def test_block_add_param6(block):
    block.add_param(Param(block, "test name", "t1"))
    try:
        block.add_param("another name", "t1")
    except exceptions.BlockSetupException as e:
        assert "not unique" in e.message
    else:
        assert False


def test_block_add_ports(block):

    block.add_message_sink("in1")
    block.add_stream_sink("in2")
    block.add_message_source("out1")
    block.add_stream_source("out2")

    block.add_port(StreamPort, 'sink', "in")
    try:
        block.add_port(StreamPort, 'up', "in")
    except exceptions.BlockSetupException:
        pass
    else:
        assert False
    block.rewrite()
