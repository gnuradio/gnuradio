# Copyright 2011 Free Software Foundation, Inc.
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

########################################################################
GR_PYTHON_INSTALL(FILES
    Block.py
    Config.py
    Connection.py
    Constants.py
    Element.py
    FlowGraph.py
    __init__.py
    odict.py
    Param.py
    ParseXML.py
    Platform.py
    Port.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/grc/core
    COMPONENT "grc"
)

install(FILES
    block.dtd
    block_tree.dtd
    default_flow_graph.grc
    domain.dtd
    flow_graph.dtd
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/grc/core
    COMPONENT "grc"
)

add_subdirectory(generator)
add_subdirectory(utils)
