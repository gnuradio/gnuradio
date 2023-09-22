import collections
import os

import codecs

from .cpp_top_block import CppTopBlockGenerator

from .. import Constants
from ..io import yaml


class CppHierBlockGenerator(CppTopBlockGenerator):
    """Extends the top block generator to also generate a block YML file"""

    def __init__(self, flow_graph, file_path):
        """
        Initialize the hier block generator object.

        Args:
            flow_graph: the flow graph object
            file_path: where to write the py file (the yml goes into HIER_BLOCK_LIB_DIR)
        """
        CppTopBlockGenerator.__init__(self, flow_graph, file_path)
        platform = flow_graph.parent

        hier_block_lib_dir = platform.config.hier_block_lib_dir
        if not os.path.exists(hier_block_lib_dir):
            os.mkdir(hier_block_lib_dir)

        self._mode = Constants.HIER_BLOCK_FILE_MODE
        self.file_path = os.path.join(
            hier_block_lib_dir, self._flow_graph.get_option('id'))
        self.file_path_yml = self.file_path + '.block.yml'

    def write(self):
        """generate output and write it to files"""
        CppTopBlockGenerator.write(self)

        data = yaml.dump(self._build_block_n_from_flow_graph_io())

        replace = [
            ('parameters:', '\nparameters:'),
            ('inputs:', '\ninputs:'),
            ('outputs:', '\noutputs:'),
            ('asserts:', '\nasserts:'),
            ('\ntemplates:', '\n\ntemplates:'),
            ('cpp_templates:', '\ncpp_templates:'),
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
        data['flags'] = ['cpp']

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
                if port.domain == Constants.GR_MESSAGE_DOMAIN:
                    p['id'] = port.id
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

        t['import'] = "from {0} import {0}  # grc-generated hier_block".format(
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
            t['make'] += '\nthis->connect(this->${id});'

        # Callback data
        t['callbacks'] = [
            'set_{key}(${{ {key} }})'.format(key=param_block.name) for param_block in parameters
        ]

        t_cpp = data['cpp_templates'] = collections.OrderedDict()

        t_cpp['includes'] = []
        t_cpp['includes'].append(
            '#include "{id}/{id}.hpp"'.format(id=self._flow_graph.get_option('id')))

        # Make data
        if parameters:
            t_cpp['make'] = '{cls}(\n    {kwargs},\n)'.format(
                cls=block_id,
                kwargs=',\n    '.join(
                    '{key}=${{ {key} }}'.format(key=param.name) for param in parameters
                ),
            )
        else:
            t_cpp['make'] = 'this->${{id}} = {cls}_sptr(make_{cls}());'.format(
                cls=block_id)
            t_cpp['declarations'] = '{cls}_sptr ${{id}};'.format(cls=block_id)

        # Callback data
        t_cpp['callbacks'] = [
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


class CppQtHierBlockGenerator(CppHierBlockGenerator):

    def _build_block_n_from_flow_graph_io(self):
        n = CppHierBlockGenerator._build_block_n_from_flow_graph_io(self)
        block_n = collections.OrderedDict()

        # insert flags after category
        for key, value in n['block'].items():
            block_n[key] = value
            if key == 'category':
                block_n['flags'] = 'need_qt_gui'

        if not block_n['name'].upper().startswith('QT GUI'):
            block_n['name'] = 'QT GUI ' + block_n['name']

        gui_hint_param = collections.OrderedDict()
        gui_hint_param['name'] = 'GUI Hint'
        gui_hint_param['key'] = 'gui_hint'
        gui_hint_param['value'] = ''
        gui_hint_param['type'] = 'gui_hint'
        gui_hint_param['hide'] = 'part'
        block_n['param'].append(gui_hint_param)

        block_n['make'] += (
            "\n#set $win = 'self.%s' % $id"
            "\n${gui_hint()($win)}"
        )

        return {'block': block_n}


def get_hier_block_io(flow_graph, direction, domain=None):
    """
    Get a list of io ports for this flow graph.

    Returns a list of dicts with: type, label, vlen, size, optional
    """
    pads = flow_graph.get_pad_sources(
    ) if direction == 'inputs' else flow_graph.get_pad_sinks()

    for pad in pads:
        for port in (pad.sources if direction == 'inputs' else pad.sinks):
            if domain and port.domain != domain:
                continue
            yield port
