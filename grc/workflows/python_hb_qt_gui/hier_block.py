"""Python code generator for Qt-GUI hier blocks.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import collections
import os

from ..python_hb_nogui import PythonHierBlockNoGuiGenerator


class PythonHierBlockQtGenerator(PythonHierBlockNoGuiGenerator):
    """Generator class for Python-based hier blocks with Qt GUIs."""

    def __init__(self, flow_graph, output_dir):
        super().__init__(
            flow_graph,
            output_dir,
            py_template=os.path.join(os.path.dirname(__file__), 'flow_graph_hb_qt_gui.py.mako')
        )

    def _build_block_n_from_flow_graph_io(self):
        n = super()._build_block_n_from_flow_graph_io()
        block_n = collections.OrderedDict()

        # insert flags after category
        for key, value in n.items():
            block_n[key] = value
            if key == 'category':
                block_n['flags'] = 'need_qt_gui'

        if not block_n['label'].upper().startswith('QT GUI'):
            block_n['label'] = 'QT GUI ' + block_n['label']

        gui_hint_param = collections.OrderedDict()
        gui_hint_param['id'] = 'gui_hint'
        gui_hint_param['label'] = 'GUI Hint'
        gui_hint_param['dtype'] = 'gui_hint'
        gui_hint_param['hide'] = 'part'
        block_n['parameters'].append(gui_hint_param)

        block_n['templates']['make'] += (
            "\n<% win = 'self.%s'%id %>"
            "\n${ gui_hint() % win }"
        )

        return block_n
