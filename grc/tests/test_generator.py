# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

from os import path
import tempfile

from grc.core.platform import Platform


def test_generator():
    # c&p form compiler code.
    # todo: make this independent from installed GR
    grc_file = path.join(path.dirname(__file__), 'resources', 'test_compiler.grc')
    out_dir = tempfile.gettempdir()

    platform = Platform(
        name='GNU Radio Companion Compiler',
        prefs=None,
        version='0.0.0',
    )
    platform.build_library()

    flow_graph = platform.make_flow_graph(grc_file)
    flow_graph.rewrite()
    flow_graph.validate()

    assert flow_graph.is_valid()

    generator = platform.Generator(flow_graph, path.join(path.dirname(__file__), 'resources'))
    generator.write()
