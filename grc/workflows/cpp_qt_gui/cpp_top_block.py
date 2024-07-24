"""C++ code generator for Qt-GUI flow graphs.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import os

from ..common import CppGeneratorBase
from ...core.Constants import TOP_BLOCK_FILE_MODE


class CppQtGuiTopBlockGenerator(CppGeneratorBase):
    """Generator for no-GUI C++ top block code."""

    def __init__(self, flow_graph, output_dir):
        """
        Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            output_dir: the path for written files
        """
        this_dir = os.path.dirname(__file__)
        super().__init__(flow_graph, output_dir, add_xterm=True,
                         header_template=os.path.join(this_dir, 'flow_graph_qt_gui.hpp.mako'),
                         source_template=os.path.join(this_dir, 'flow_graph_qt_gui.cpp.mako'),
                         cmake_template=os.path.join(this_dir, 'CMakeLists.txt.mako'),
                         )
        self._mode = TOP_BLOCK_FILE_MODE
