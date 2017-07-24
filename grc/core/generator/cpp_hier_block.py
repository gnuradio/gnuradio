import collections
import os

import six

from .top_block import TopBlockGenerator

from .. import ParseXML, Constants

class CppHierBlockGenerator(CppTopBlockGenerator):
    """Extends the top block generator to also generate a block XML file"""

    def __init__(self, flow_graph, file_path):
        """
        Initialize the hier block generator object.

        Args:
            flow_graph: the flow graph object
            file_path: where to write the py file (the xml goes into HIER_BLOCK_LIB_DIR)
        """
        CppTopBlockGenerator.__init__(self, flow_graph, file_path)
        platform = flow_graph.parent

        hier_block_lib_dir = platform.config.hier_block_lib_dir
        if not os.path.exists(hier_block_lib_dir):
            os.mkdir(hier_block_lib_dir)

        self._mode = Constants.HIER_BLOCK_FILE_MODE
        self.file_path = os.path.join(hier_block_lib_dir, self._flow_graph.get_option('id'))
        self.file_path_xml = self.file_path + '.xml' # TODO: yml

    def write(self):
        """generate output and write it to files"""
        TopBlockGenerator.write(self)
        ParseXML.to_file(self._build_block_n_from_flow_graph_io(), self.file_path_xml)
        ParseXML.validate_dtd(self.file_path_xml, Constants.BLOCK_DTD)
        try:
            os.chmod(self.file_path_xml, self._mode)
        except:
            pass

    def _build_block_n_from_flow_graph_io(self):
        """
        Generate a block XML nested data from the flow graph IO

        Returns:
            a xml node tree
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
            p['dtype'] = 'raw'
            p['value'] = param_block.params['value'].get_value()
            p['hide'] = param_block.params['hide'].get_value()
            data['param'].append(p)

        # Ports
        for direction in ('inputs', 'outputs'):
            data[direction] = []
            for port in get_hier_block_io(self._flow_graph, direction):
                p = collections.OrderedDict()
                if port.domain == Constants.GR_MESSAGE_DOMAIN:
                    p['id'] = port.id
                #p['label'] = port.label
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
                    '{key}=${key}'.format(key=param.name) for param in parameters
                ),
            )
        else:
            t['make'] = '{cls}()'.format(cls=block_id)
        # Callback data
        t['callback'] = [
            'set_{key}(${key})'.format(key=param_block.name) for param_block in parameters
        ]


        # Documentation
        data['doc'] = "\n".join(field for field in (
            self._flow_graph.get_option('author'),
            self._flow_graph.get_option('description'),
            self.file_path
        ) if field)
        data['grc_source'] = str(self._flow_graph.grc_file_path)

        n = {'block': data}
        return n


class CppQtHierBlockGenerator(CppHierBlockGenerator):

    def _build_block_n_from_flow_graph_io(self):
        n = CppHierBlockGenerator._build_block_n_from_flow_graph_io(self)
        block_n = collections.OrderedDict()

        # insert flags after category
        for key, value in six.iteritems(n['block']):
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
    pads = flow_graph.get_pad_sources() if direction == 'inputs' else flow_graph.get_pad_sinks()

    ports = []
    for pad in pads:
        for port in (pad.sources if direction == 'inputs' else pad.sinks):
            if domain and port.domain != domain:
                continue
            yield port

        type_param = pad.params['type']
        master = {
            'label': str(pad.params['label'].get_evaluated()),
            'type': str(pad.params['type'].get_evaluated()),
            'vlen': str(pad.params['vlen'].get_value()),
            'size':  type_param.options.attributes[type_param.get_value()]['size'],
            'optional': bool(pad.params['optional'].get_evaluated()),
        }

        if domain and pad:
            num_ports = pad.params['num_streams'].get_evaluated()
            if num_ports <= 1:
                yield master
            else:
                for i in range(num_ports):
                    clone = master.copy()
                    clone['label'] += str(i)
                    ports.append(clone)
        else:
            ports.append(master)

    if domain is not None:
        ports = [p for p in ports if p.domain == domain]
        yield ports  # TODO: Not sure this fix is correct
