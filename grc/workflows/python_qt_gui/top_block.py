"""Generator for Python-Qt-GUI flow graphs.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import os

from ..common import PythonGeneratorBase
from ...core.Constants import TOP_BLOCK_FILE_MODE


class PythonQtGuiGenerator(PythonGeneratorBase):
    """Generator for Python code with Qt GUI."""

    def __init__(self, flow_graph, output_dir):
        """Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            output_dir: the path for written files
        """
        super().__init__(
            flow_graph,
            output_dir,
            add_xterm=False,
            py_template=os.path.join(
                os.path.dirname(__file__),
                "flow_graph_qt_gui.py.mako"))
        self._mode = TOP_BLOCK_FILE_MODE

    def write(self, _=None):
        """generate output and write it to files"""
        self._warnings()

        fg = self._flow_graph
        self.title = fg.get_option('title') or fg.get_option(
            'id').replace('_', ' ').title()

        for filename, data in self._build_python_code_from_template():
            with open(filename, 'w', encoding='utf-8') as fp:
                fp.write(data)
            if filename == self.file_path:
                os.chmod(filename, self._mode)
