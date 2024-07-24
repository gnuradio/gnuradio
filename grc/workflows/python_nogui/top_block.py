"""Generator for Python-based flow graphs without GUIs.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import os

from ..common import PythonGeneratorBase
from ...core.Constants import TOP_BLOCK_FILE_MODE


class PythonNoGuiGenerator(PythonGeneratorBase):
    """Generator class for Python no-gui applications."""

    def __init__(self, flow_graph, output_dir, py_template=None):
        """Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            output_dir: the path for written files
        """
        super().__init__(
            flow_graph,
            output_dir,
            add_xterm=True,
            py_template=py_template or os.path.join(
                os.path.dirname(__file__),
                'flow_graph_nogui.py.mako'))
        self._mode = TOP_BLOCK_FILE_MODE

    def write(self, _=None):
        """Generate output and write it to files."""
        self._warnings()

        fg = self._flow_graph
        self.title = fg.get_option('title') or fg.get_option(
            'id').replace('_', ' ').title()
        variables = fg.get_variables()
        parameters = fg.get_parameters()
        monitors = fg.get_monitors()

        self.namespace = {
            'flow_graph': fg,
            'variables': variables,
            'parameters': parameters,
            'monitors': monitors,
            'generate_options': self.generate_options,
        }

        for filename, data in self._build_python_code_from_template():
            with open(filename, 'w', encoding='utf-8') as fp:
                fp.write(data)
            if filename == self.file_path:
                os.chmod(filename, self._mode)
