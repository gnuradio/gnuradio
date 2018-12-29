import codecs
import yaml
import operator
import os
import tempfile
import textwrap
import time
import re

from mako.template import Template

from .. import Messages, blocks
from ..Constants import TOP_BLOCK_FILE_MODE
from .FlowGraphProxy import FlowGraphProxy
from ..utils import expr_utils
from .top_block import TopBlockGenerator

DATA_DIR = os.path.dirname(__file__)

HEADER_TEMPLATE = os.path.join(DATA_DIR, 'cpp_templates/flow_graph.hpp.mako')
SOURCE_TEMPLATE = os.path.join(DATA_DIR, 'cpp_templates/flow_graph.cpp.mako')
CMAKE_TEMPLATE = os.path.join(DATA_DIR, 'cpp_templates/CMakeLists.txt.mako')

header_template = Template(filename=HEADER_TEMPLATE)
source_template = Template(filename=SOURCE_TEMPLATE)
cmake_template = Template(filename=CMAKE_TEMPLATE)


class CppTopBlockGenerator(TopBlockGenerator):

    def __init__(self, flow_graph, file_path):
        """
        Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            file_path: the path to write the file to
        """

        self._flow_graph = FlowGraphProxy(flow_graph)
        self._generate_options = self._flow_graph.get_option('generate_options')

        self._mode = TOP_BLOCK_FILE_MODE
        dirname = os.path.dirname(file_path)
        # Handle the case where the directory is read-only
        # In this case, use the system's temp directory
        if not os.access(dirname, os.W_OK):
            dirname = tempfile.gettempdir()

        filename = self._flow_graph.get_option('id')
        self.file_path = os.path.join(dirname, filename)
        self._dirname = dirname

    def write(self):
        """generate output and write it to files"""
        self._warnings()

        fg = self._flow_graph
        self.title = fg.get_option('title') or fg.get_option('id').replace('_', ' ').title()
        variables = fg.get_cpp_variables()
        parameters = fg.get_parameters()
        monitors = fg.get_monitors()
        self._variable_types()
        self._parameter_types()

        self.namespace = {
            'flow_graph': fg,
            'variables': variables,
            'parameters': parameters,
            'monitors': monitors,
            'generate_options': self._generate_options,
            'generated_time': time.ctime(),
        }

        if not os.path.exists(self.file_path):
            os.makedirs(self.file_path)

        for filename, data in self._build_cpp_header_code_from_template():
            with codecs.open(filename, 'w', encoding='utf-8') as fp:
                fp.write(data)

        if not self._generate_options.startswith('hb'):
            if not os.path.exists(os.path.join(self.file_path, 'build')):
                os.makedirs(os.path.join(self.file_path, 'build'))

            for filename, data in self._build_cpp_source_code_from_template():
                with codecs.open(filename, 'w', encoding='utf-8') as fp:
                    fp.write(data)

            if fg.get_option('gen_cmake') == 'On':
                for filename, data in self._build_cmake_code_from_template():
                    with codecs.open(filename, 'w', encoding='utf-8') as fp:
                        fp.write(data)

    def _build_cpp_source_code_from_template(self):
        """
        Convert the flow graph to a C++ source file.

        Returns:
            a string of C++ code
        """
        file_path = self.file_path + '/' + self._flow_graph.get_option('id') + '.cpp'

        output = []

        flow_graph_code = source_template.render(
            title=self.title,
            includes=self._includes(),
            blocks=self._blocks(),
            callbacks=self._callbacks(),
            connections=self._connections(),
            **self.namespace
        )
        # strip trailing white-space
        flow_graph_code = "\n".join(line.rstrip() for line in flow_graph_code.split("\n"))
        output.append((file_path, flow_graph_code))

        return output


    def _build_cpp_header_code_from_template(self):
        """
        Convert the flow graph to a C++ header file.

        Returns:
            a string of C++ code
        """
        file_path = self.file_path + '/' + self._flow_graph.get_option('id') + '.hpp'

        output = []

        flow_graph_code = header_template.render(
            title=self.title,
            includes=self._includes(),
            blocks=self._blocks(),
            callbacks=self._callbacks(),
            connections=self._connections(),
            **self.namespace
        )
        # strip trailing white-space
        flow_graph_code = "\n".join(line.rstrip() for line in flow_graph_code.split("\n"))
        output.append((file_path, flow_graph_code))

        return output

    def _build_cmake_code_from_template(self):
        """
        Convert the flow graph to a CMakeLists.txt file.

        Returns:
            a string of CMake code
        """
        filename = 'CMakeLists.txt'
        file_path = os.path.join(self.file_path, filename)

        output = []

        flow_graph_code = cmake_template.render(
            title=self.title,
            includes=self._includes(),
            blocks=self._blocks(),
            callbacks=self._callbacks(),
            connections=self._connections(),
            links=self._links(),
            **self.namespace
        )
        # strip trailing white-space
        flow_graph_code = "\n".join(line.rstrip() for line in flow_graph_code.split("\n"))
        output.append((file_path, flow_graph_code))

        return output

    def _links(self):
        fg = self._flow_graph
        links = fg.links()
        seen = set()
        output = []

        for link_list in links:
            if link_list:
                for link in link_list:
                    seen.add(link)

        return list(seen)

    def _includes(self):
        fg = self._flow_graph
        includes = fg.includes()
        seen = set()
        output = []

        def is_duplicate(l):
            if l.startswith('#include') and l in seen:
                return True
            seen.add(line)
            return False

        for block_ in includes:
            for include_ in block_:
                if not include_:
                    continue
                line = include_.rstrip()
                if not is_duplicate(line):
                    output.append(line)

        return output

    def _blocks(self):
        fg = self._flow_graph
        parameters = fg.get_parameters()

        # List of blocks not including variables and imports and parameters and disabled
        def _get_block_sort_text(block):
            code = block.cpp_templates.render('make').replace(block.name, ' ')
            try:
                code += block.params['gui_hint'].get_value()  # Newer gui markup w/ qtgui
            except:
                pass
            return code

        blocks = [
            b for b in fg.blocks
            if b.enabled and not (b.get_bypassed() or b.is_import or b in parameters or b.key == 'options')
        ]

        blocks = expr_utils.sort_objects(blocks, operator.attrgetter('name'), _get_block_sort_text)
        blocks_make = []
        for block in blocks:
            translations = block.cpp_templates.render('translations')
            make = block.cpp_templates.render('make')
            declarations = block.cpp_templates.render('declarations')
            if translations:
                translations = yaml.load(translations)
            else:
                translations = {}
            translations.update(
                {r"gr\.sizeof_([\w_]+)": r"sizeof(\1)"}
            )
            for key in translations:
                make = re.sub(key.replace("\\\\", "\\"), translations[key],make)
                declarations = declarations.replace(key, translations[key])
            if make:
                blocks_make.append((block, make, declarations))
            elif 'qt' in block.key:
                # The QT Widget blocks are technically variables,
                # but they contain some code we don't want to miss
                blocks_make.append(('', make, declarations))
        return blocks_make

    def _variable_types(self):
        fg = self._flow_graph
        variables = fg.get_cpp_variables()

        for var in variables:
            var.decide_type()

    def _parameter_types(self):
        fg = self._flow_graph
        parameters = fg.get_parameters()

        for param in parameters:
            type_translation = {'eng_float' : 'double', 'intx' : 'int', 'std' : 'std::string'};
            param.vtype = type_translation[param.params['type'].value]

    def _callbacks(self):
        fg = self._flow_graph
        variables = fg.get_cpp_variables()
        parameters = fg.get_parameters()

        # List of variable names
        var_ids = [var.name for var in parameters + variables]

        replace_dict = dict((var_id, 'this->' + var_id) for var_id in var_ids)

        callbacks_all = []
        for block in fg.iter_enabled_blocks():
            callbacks_all.extend(expr_utils.expr_replace(cb, replace_dict) for cb in block.get_cpp_callbacks())

        # Map var id to callbacks
        def uses_var_id(callback):
            used = expr_utils.get_variable_dependencies(callback, [var_id])
            return used and ('this->' + var_id in callback)  # callback might contain var_id itself

        callbacks = {}
        for var_id in var_ids:
            callbacks[var_id] = [callback for callback in callbacks_all if uses_var_id(callback)]

        return callbacks

    def _connections(self):
        fg = self._flow_graph
        templates = {key: Template(text)
                     for key, text in fg.parent_platform.cpp_connection_templates.items()}

        def make_port_sig(port):
            if port.parent.key in ('pad_source', 'pad_sink'):
                block = 'self()'
                key = fg.get_pad_port_global_key(port)
            else:
                block = 'this->' + port.parent_block.name
                key = port.key

            if not key.isdigit():
                key = re.findall(r'\d+', key)[0]

            return '{block}, {key}'.format(block=block, key=key)

        connections = fg.get_enabled_connections()

        # Get the virtual blocks and resolve their connections
        connection_factory = fg.parent_platform.Connection
        virtual = [c for c in connections if isinstance(c.source_block, blocks.VirtualSource)]
        for connection in virtual:
            sink = connection.sink_port
            for source in connection.source_port.resolve_virtual_source():
                resolved = connection_factory(fg.orignal_flowgraph, source, sink)
                connections.append(resolved)
            # Remove the virtual connection
            connections.remove(connection)

        # Bypassing blocks: Need to find all the enabled connections for the block using
        # the *connections* object rather than get_connections(). Create new connections
        # that bypass the selected block and remove the existing ones. This allows adjacent
        # bypassed blocks to see the newly created connections to downstream blocks,
        # allowing them to correctly construct bypass connections.
        bypassed_blocks = fg.get_bypassed_blocks()
        for block in bypassed_blocks:
            # Get the upstream connection (off of the sink ports)
            # Use *connections* not get_connections()
            source_connection = [c for c in connections if c.sink_port == block.sinks[0]]
            # The source connection should never have more than one element.
            assert (len(source_connection) == 1)

            # Get the source of the connection.
            source_port = source_connection[0].source_port

            # Loop through all the downstream connections
            for sink in (c for c in connections if c.source_port == block.sources[0]):
                if not sink.enabled:
                    # Ignore disabled connections
                    continue
                connection = connection_factory(fg.orignal_flowgraph, source_port, sink.sink_port)
                connections.append(connection)
                # Remove this sink connection
                connections.remove(sink)
            # Remove the source connection
            connections.remove(source_connection[0])

        # List of connections where each endpoint is enabled (sorted by domains, block names)
        def by_domain_and_blocks(c):
            return c.type, c.source_block.name, c.sink_block.name

        rendered = []
        for con in sorted(connections, key=by_domain_and_blocks):
            template = templates[con.type]
            code = template.render(make_port_sig=make_port_sig, source=con.source_port, sink=con.sink_port)
            if not self._generate_options.startswith('hb'):
                code = 'this->tb->' + code
            rendered.append(code)

        return rendered
