"""Python code generator for no-GUI hier blocks.

SPDX-License-Identifier: GPL-3.0-or-later
"""

import codecs
import collections
import os

from ..common import HierBlockGeneratorMixin
from ..common import get_hier_block_io
from ..python_nogui.top_block import PythonNoGuiGenerator
from ...core import Constants
from ...core.io import yaml


class PythonHierBlockNoGuiGenerator(HierBlockGeneratorMixin, PythonNoGuiGenerator):
    """Extends the top block generator to also generate a block YML file"""

    def __init__(self, flow_graph, output_dir, py_template=None):
        """
        Initialize the hier block generator object.

        Args:
            flow_graph: the flow graph object
            output_dir: the path for written files
        """
        py_template = py_template or os.path.join(
            os.path.dirname(__file__), "flow_graph_hb_nogui.py.mako"
        )
        super().__init__(flow_graph, self.get_output_dir_for_hier_block(flow_graph), py_template=py_template)

        self._mode = Constants.HIER_BLOCK_FILE_MODE
        self.file_path_yml = os.path.splitext(self.file_path)[0] + '.block.yml'

    def write(self, _=None):
        """generate output and write it to files"""
        super().write(self)

        data = yaml.dump(self._build_block_n_from_flow_graph_io())

        replace = [
            ('parameters:', '\nparameters:'),
            ('inputs:', '\ninputs:'),
            ('outputs:', '\noutputs:'),
            ('asserts:', '\nasserts:'),
            ('templates:', '\ntemplates:'),
            ('documentation:', '\ndocumentation:'),
            ('file_format:', '\nfile_format:'),
        ]
        for r in replace:
            data = data.replace(*r)

        with codecs.open(self.file_path_yml, 'w', encoding='utf-8') as fp:
            fp.write(data)

        # Windows only supports S_IREAD and S_IWRITE, other flags are ignored
        os.chmod(self.file_path_yml, self._mode)

    def _build_block_n_from_flow_graph_io(self):
        """
        Generate a block YML nested data from the flow graph IO

        Returns:
            a yml node tree
        """
        # Extract info from the flow graph
        block_id = self._flow_graph.get_option('id')
        parameters = self._flow_graph.get_parameters()

        def var_or_value(name):
            if name in (p.name for p in parameters):
                return "${" + name + " }"
            return name

        # Build the nested data
        data = collections.OrderedDict()
        data['id'] = block_id
        data['label'] = (
            self._flow_graph.get_option('title') or
            self._flow_graph.get_option('id').replace('_', ' ').title()
        )
        data['category'] = self._flow_graph.get_option('category')

        # Parameters
        data['parameters'] = []
        for param_block in parameters:
            p = collections.OrderedDict()
            p['id'] = param_block.name
            p['label'] = param_block.params['label'].get_value() or param_block.name
            p['dtype'] = param_block.params['value'].dtype
            p['default'] = param_block.params['value'].get_value()
            p['hide'] = param_block.params['hide'].get_value()
            data['parameters'].append(p)

        # Ports
        for direction in ('inputs', 'outputs'):
            data[direction] = []
            for port in get_hier_block_io(self._flow_graph, direction):
                p = collections.OrderedDict()
                p['label'] = port.parent.params['label'].value
                if port.domain != Constants.DEFAULT_DOMAIN:
                    p['domain'] = port.domain
                p['dtype'] = port.dtype
                if port.domain != Constants.GR_MESSAGE_DOMAIN:
                    p['vlen'] = var_or_value(port.vlen)
                if port.optional:
                    p['optional'] = True
                data[direction].append(p)

        t = data['templates'] = collections.OrderedDict()

        t['imports'] = "from {0} import {0}  # grc-generated hier_block".format(
            self._flow_graph.get_option('id'))
        # Make data
        if parameters:
            t['make'] = '{cls}(\n    {kwargs},\n)'.format(
                cls=block_id,
                kwargs=',\n    '.join(
                    '{key}=${{ {key} }}'.format(key=param.name) for param in parameters
                ),
            )
        else:
            t['make'] = '{cls}()'.format(cls=block_id)
        # Self-connect if there aren't any ports
        if not data['inputs'] and not data['outputs']:
            t['make'] += '\nself.connect(self.${id})'

        # Callback data
        t['callbacks'] = [
            'set_{key}(${{ {key} }})'.format(key=param_block.name) for param_block in parameters
        ]

        # Documentation
        data['documentation'] = "\n".join(field for field in (
            self._flow_graph.get_option('author'),
            self._flow_graph.get_option('description'),
            self.file_path
        ) if field)
        data['grc_source'] = str(self._flow_graph.grc_file_path)

        data['file_format'] = 1

        return data
