import codecs
import operator
import os
import tempfile
import textwrap

from mako.template import Template

from .. import Messages, blocks
from ..Constants import TOP_BLOCK_FILE_MODE
from .FlowGraphProxy import FlowGraphProxy
from ..utils import expr_utils

DATA_DIR = os.path.dirname(__file__)

PYTHON_TEMPLATE = os.path.join(DATA_DIR, 'flow_graph.py.mako')

python_template = Template(filename=PYTHON_TEMPLATE)


class TopBlockGenerator(object):

    def __init__(self, flow_graph, output_dir):
        """
        Initialize the top block generator object.

        Args:
            flow_graph: the flow graph object
            output_dir: the path for written files
        """

        self._flow_graph = FlowGraphProxy(flow_graph)
        self._generate_options = self._flow_graph.get_option(
            'generate_options')

        self._mode = TOP_BLOCK_FILE_MODE
        # Handle the case where the directory is read-only
        # In this case, use the system's temp directory
        if not os.access(output_dir, os.W_OK):
            output_dir = tempfile.gettempdir()
        filename = self._flow_graph.get_option('id') + '.py'
        self.file_path = os.path.join(output_dir, filename)
        self.output_dir = output_dir

    def _warnings(self):
        throttling_blocks = [b for b in self._flow_graph.get_enabled_blocks()
                             if b.flags.throttle]
        if not throttling_blocks and not self._generate_options.startswith('hb'):
            Messages.send_warning("This flow graph may not have flow control: "
                                  "no audio or RF hardware blocks found. "
                                  "Add a Misc->Throttle block to your flow "
                                  "graph to avoid CPU congestion.")
        if len(throttling_blocks) > 1:
            keys = set([b.key for b in throttling_blocks])
            if len(keys) > 1 and 'blocks_throttle' in keys:
                Messages.send_warning("This flow graph contains a throttle "
                                      "block and another rate limiting block, "
                                      "e.g. a hardware source or sink. "
                                      "This is usually undesired. Consider "
                                      "removing the throttle block.")

        deprecated_block_keys = {
            b.name for b in self._flow_graph.get_enabled_blocks() if b.flags.deprecated}
        for key in deprecated_block_keys:
            Messages.send_warning("The block {!r} is deprecated.".format(key))

    def write(self):
        """generate output and write it to files"""
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
            'generate_options': self._generate_options,
        }

        for filename, data in self._build_python_code_from_template():
            with codecs.open(filename, 'w', encoding='utf-8') as fp:
                fp.write(data)
            if filename == self.file_path:
                os.chmod(filename, self._mode)

    def _build_python_code_from_template(self):
        """
        Convert the flow graph to python code.

        Returns:
            a string of python code
        """
        output = []

        fg = self._flow_graph
        platform = fg.parent
        title = fg.get_option('title') or fg.get_option(
            'id').replace('_', ' ').title()
        variables = fg.get_variables()
        parameters = fg.get_parameters()
        monitors = fg.get_monitors()

        for block in fg.iter_enabled_blocks():
            if block.key == 'epy_block':
                src = block.params['_source_code'].get_value()
            elif block.key == 'epy_module':
                src = block.params['source_code'].get_value()
            else:
                continue

            file_path = os.path.join(
                self.output_dir, block.module_name + ".py")
            output.append((file_path, src))

        self.namespace = {
            'flow_graph': fg,
            'variables': variables,
            'parameters': parameters,
            'monitors': monitors,
            'generate_options': self._generate_options,
            'version': platform.config.version,
            'catch_exceptions': fg.get_option('catch_exceptions')
        }
        flow_graph_code = python_template.render(
            title=title,
            imports=self._imports(),
            blocks=self._blocks(),
            callbacks=self._callbacks(),
            connections=self._connections(),
            **self.namespace
        )
        # strip trailing white-space
        flow_graph_code = "\n".join(line.rstrip()
                                    for line in flow_graph_code.split("\n"))
        output.append((self.file_path, flow_graph_code))

        return output

    def _imports(self):
        fg = self._flow_graph
        imports = fg.imports()
        seen = set()
        output = []

        need_path_hack = any(imp.endswith(
            "# grc-generated hier_block") for imp in imports)
        if need_path_hack:
            output.insert(0, textwrap.dedent("""\
                import os
                import sys
                sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))
            """))
            seen.add('import os')
            seen.add('import sys')

        if fg.get_option('qt_qss_theme'):
            imports.append('import os')
            imports.append('import sys')

        if fg.get_option('thread_safe_setters'):
            imports.append('import threading')

        def is_duplicate(l):
            if (l.startswith('import') or l.startswith('from')) and l in seen:
                return True
            seen.add(line)
            return False

        for import_ in sorted(imports):
            lines = import_.strip().split('\n')
            if not lines[0]:
                continue
            for line in lines:
                line = line.rstrip()
                if not is_duplicate(line):
                    output.append(line)

        return output

    def _blocks(self):
        fg = self._flow_graph
        parameters = fg.get_parameters()

        # List of blocks not including variables and imports and parameters and disabled
        def _get_block_sort_text(block):
            code = block.templates.render('make').replace(block.name, ' ')
            try:
                # Newer gui markup w/ qtgui
                code += block.params['gui_hint'].get_value()
            except KeyError:
                # No gui hint
                pass
            return code

        blocks = [
            b for b in fg.blocks
            if b.enabled and not (b.get_bypassed() or b.is_import or b.is_snippet or b in parameters or b.key == 'options')
        ]

        blocks = expr_utils.sort_objects(
            blocks, operator.attrgetter('name'), _get_block_sort_text)
        blocks_make = []
        for block in blocks:
            make = block.templates.render('make')
            if make:
                if not (block.is_variable or block.is_virtual_or_pad):
                    make = 'self.' + block.name + ' = ' + make
                blocks_make.append((block, make))
        return blocks_make

    def _callbacks(self):
        fg = self._flow_graph
        variables = fg.get_variables()
        parameters = fg.get_parameters()

        # List of variable names
        var_ids = [var.name for var in parameters + variables]

        replace_dict = dict((var_id, 'self.' + var_id) for var_id in var_ids)

        callbacks_all = []
        for block in fg.iter_enabled_blocks():
            callbacks_all.extend(expr_utils.expr_replace(
                cb, replace_dict) for cb in block.get_callbacks())

        # Map var id to callbacks
        def uses_var_id(callback):
            used = expr_utils.get_variable_dependencies(callback, [var_id])
            # callback might contain var_id itself
            return used and (('self.' + var_id in callback) or ('this->' + var_id in callback))

        callbacks = {}
        for var_id in var_ids:
            callbacks[var_id] = [
                callback for callback in callbacks_all if uses_var_id(callback)]

        return callbacks

    def _connections(self):
        fg = self._flow_graph
        templates = {key: Template(text)
                     for key, text in fg.parent_platform.connection_templates.items()}

        def make_port_sig(port):
            # TODO: make sense of this
            if port.parent.key in ('pad_source', 'pad_sink'):
                block = 'self'
                key = fg.get_pad_port_global_key(port)
            else:
                block = 'self.' + port.parent_block.name
                key = port.key

            if not key.isdigit():
                key = repr(key)

            return '({block}, {key})'.format(block=block, key=key)

        connections = fg.get_enabled_connections()

        # Get the virtual blocks and resolve their connections
        connection_factory = fg.parent_platform.Connection
        virtual_source_connections = [c for c in connections if isinstance(
            c.source_block, blocks.VirtualSource)]
        for connection in virtual_source_connections:
            sink = connection.sink_port
            for source in connection.source_port.resolve_virtual_source():
                resolved = connection_factory(
                    fg.orignal_flowgraph, source, sink)
                connections.append(resolved)

        virtual_connections = [c for c in connections if (isinstance(
            c.source_block, blocks.VirtualSource) or isinstance(c.sink_block, blocks.VirtualSink))]
        for connection in virtual_connections:
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
            source_connection = [
                c for c in connections if c.sink_port == block.sinks[0]]
            # The source connection should never have more than one element.
            assert (len(source_connection) == 1)

            # Get the source of the connection.
            source_port = source_connection[0].source_port

            # Loop through all the downstream connections
            for sink in (c for c in connections if c.source_port == block.sources[0]):
                if not sink.enabled:
                    # Ignore disabled connections
                    continue
                connection = connection_factory(
                    fg.orignal_flowgraph, source_port, sink.sink_port)
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
            if con.source_port.dtype != 'bus':
                code = template.render(
                    make_port_sig=make_port_sig, source=con.source_port, sink=con.sink_port)
                rendered.append(code)
            else:
                # Bus ports need to iterate over the underlying connections and then render
                # the code for each subconnection
                porta = con.source_port
                portb = con.sink_port
                fg = self._flow_graph

                if porta.dtype == 'bus' and portb.dtype == 'bus':
                    # which bus port is this relative to the bus structure
                    if len(porta.bus_structure) == len(portb.bus_structure):
                        for port_num_a, port_num_b in zip(porta.bus_structure, portb.bus_structure):
                            hidden_porta = porta.parent.sources[port_num_a]
                            hidden_portb = portb.parent.sinks[port_num_b]
                            connection = fg.parent_platform.Connection(
                                parent=self, source=hidden_porta, sink=hidden_portb)
                            code = template.render(
                                make_port_sig=make_port_sig, source=hidden_porta, sink=hidden_portb)
                            rendered.append(code)

        return rendered
