import codecs
import yaml
import operator
import os
import tempfile
import textwrap
import re
import ast

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


class CppTopBlockGenerator(object):

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
        filename = self._flow_graph.get_option('id')
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
        """create directory, generate output and write it to files"""
        self._warnings()

        fg = self._flow_graph
        platform = fg.parent
        self.title = fg.get_option('title') or fg.get_option(
            'id').replace('_', ' ').title()
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
            'config': platform.config
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
        file_path = self.file_path + '/' + \
            self._flow_graph.get_option('id') + '.cpp'

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
        flow_graph_code = "\n".join(line.rstrip()
                                    for line in flow_graph_code.split("\n"))
        output.append((file_path, flow_graph_code))

        return output

    def _build_cpp_header_code_from_template(self):
        """
        Convert the flow graph to a C++ header file.

        Returns:
            a string of C++ code
        """
        file_path = self.file_path + '/' + \
            self._flow_graph.get_option('id') + '.hpp'

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
        flow_graph_code = "\n".join(line.rstrip()
                                    for line in flow_graph_code.split("\n"))
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

        cmake_tuples = []
        cmake_opt = self._flow_graph.get_option("cmake_opt")
        cmake_opt = " " + cmake_opt  # To make sure we get rid of the "-D"s when splitting

        for opt_string in cmake_opt.split(" -D"):
            opt_string = opt_string.strip()
            if opt_string:
                cmake_tuples.append(tuple(opt_string.split("=")))

        output = []

        flow_graph_code = cmake_template.render(
            title=self.title,
            includes=self._includes(),
            blocks=self._blocks(),
            callbacks=self._callbacks(),
            connections=self._connections(),
            links=self._links(),
            cmake_tuples=cmake_tuples,
            packages=self._packages(),
            **self.namespace
        )
        # strip trailing white-space
        flow_graph_code = "\n".join(line.rstrip()
                                    for line in flow_graph_code.split("\n"))
        output.append((file_path, flow_graph_code))

        return output

    def _links(self):
        fg = self._flow_graph
        links = fg.links()
        seen = set()

        for link_list in links:
            if link_list:
                for link in link_list:
                    seen.add(link)

        return list(seen)

    def _packages(self):
        fg = self._flow_graph
        packages = fg.packages()
        seen = set()

        for package_list in packages:
            if package_list:
                for package in package_list:
                    seen.add(package)

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

        # List of blocks not including variables and parameters and disabled
        def _get_block_sort_text(block):
            code = block.cpp_templates.render('declarations')
            try:
                # Newer gui markup w/ qtgui
                code += block.params['gui_hint'].get_value()
            except:
                pass
            return code

        blocks = [
            b for b in fg.blocks
            if b.enabled and not (b.get_bypassed() or b.is_import or b in parameters or b.key == 'options' or b.is_virtual_source() or b.is_virtual_sink())
        ]

        blocks = expr_utils.sort_objects(
            blocks, operator.attrgetter('name'), _get_block_sort_text)
        blocks_make = []
        for block in blocks:
            translations = block.cpp_templates.render('translations')
            make = block.cpp_templates.render('make')
            declarations = block.cpp_templates.render('declarations')
            if translations:
                translations = yaml.safe_load(translations)
            else:
                translations = {}
            translations.update(
                {r"gr\.sizeof_([\w_]+)": r"sizeof(\1)"}
            )
            for key in translations:
                make = re.sub(key.replace("\\\\", "\\"),
                              translations[key], make)
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

        type_translation = {'complex': 'gr_complex', 'real': 'double', 'float': 'float', 'int': 'int', 'complex_vector': 'std::vector<gr_complex>',
                            'real_vector': 'std::vector<double>', 'float_vector': 'std::vector<float>', 'int_vector': 'std::vector<int>', 'string': 'std::string', 'bool': 'bool'}
        # If the type is explcitly specified, translate to the corresponding C++ type
        for var in list(variables):
            if var.params['value'].dtype != 'raw':
                var.vtype = type_translation[var.params['value'].dtype]
                variables.remove(var)

        # If the type is 'raw', we'll need to evaluate the variable to infer the type.
        # Create an executable fragment of code containing all 'raw' variables in
        # order to infer the lvalue types.
        #
        # Note that this differs from using ast.literal_eval() as literal_eval evaluates one
        # variable at a time. The code fragment below evaluates all variables together which
        # allows the variables to reference each other (i.e. a = b * c).
        prog = 'def get_decl_types():\n'
        prog += '\tvar_types = {}\n'
        for var in variables:
            prog += '\t' + str(var.params['id'].value) + \
                '=' + str(var.params['value'].value) + '\n'
        prog += '\tvar_types = {}\n'
        for var in variables:
            prog += '\tvar_types[\'' + str(var.params['id'].value) + \
                '\'] = type(' + str(var.params['id'].value) + ')\n'
        prog += '\treturn var_types'

        # Execute the code fragment in a separate namespace and retrieve the lvalue types
        var_types = {}
        namespace = {}
        try:
            exec(prog, namespace)
            var_types = namespace['get_decl_types']()
        except Exception as excp:
            print('Failed to get parameter lvalue types: %s' % (excp))

        # Format the rvalue of each variable expression
        for var in variables:
            var.format_expr(var_types[str(var.params['id'].value)])

    def _parameter_types(self):
        fg = self._flow_graph
        parameters = fg.get_parameters()

        for param in parameters:
            type_translation = {'eng_float': 'double', 'intx': 'int',
                                'str': 'std::string', 'complex': 'gr_complex'}
            param.vtype = type_translation[param.params['type'].value]

            if param.vtype == 'gr_complex':
                evaluated = ast.literal_eval(
                    param.params['value'].value.strip())
                cpp_cmplx = '{' + str(evaluated.real) + \
                    ', ' + str(evaluated.imag) + '}'

                # Update the 'var_make' entry in the cpp_templates dictionary
                d = param.cpp_templates
                cpp_expr = d['var_make'].replace('${value}', cpp_cmplx)
                d.update({'var_make': cpp_expr})
                param.cpp_templates = d

    def _callbacks(self):
        fg = self._flow_graph
        variables = fg.get_cpp_variables()
        parameters = fg.get_parameters()

        # List of variable names
        var_ids = [var.name for var in parameters + variables]

        replace_dict = dict((var_id, 'this->' + var_id) for var_id in var_ids)

        callbacks_all = []
        for block in fg.iter_enabled_blocks():
            if not (block.is_virtual_sink() or block.is_virtual_source()):
                callbacks_all.extend(expr_utils.expr_replace(
                    cb, replace_dict) for cb in block.get_cpp_callbacks())

        # Map var id to callbacks
        def uses_var_id(callback):
            used = expr_utils.get_variable_dependencies(callback, [var_id])
            # callback might contain var_id itself
            return used and ('this->' + var_id in callback)

        callbacks = {}
        for var_id in var_ids:
            callbacks[var_id] = [
                callback for callback in callbacks_all if uses_var_id(callback)]

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
                # TODO What use case is this supporting?
                toks = re.findall(r'\d+', key)
                if len(toks) > 0:
                    key = toks[0]
                else:
                    # Assume key is a string
                    key = '"' + key + '"'

            return '{block}, {key}'.format(block=block, key=key)

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
                if not self._generate_options.startswith('hb'):
                    code = 'this->tb->' + code
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
                        for port_num in porta.bus_structure:
                            hidden_porta = porta.parent.sources[port_num]
                            hidden_portb = portb.parent.sinks[port_num]
                            connection = fg.parent_platform.Connection(
                                parent=self, source=hidden_porta, sink=hidden_portb)
                            code = template.render(
                                make_port_sig=make_port_sig, source=hidden_porta, sink=hidden_portb)
                            if not self._generate_options.startswith('hb'):
                                code = 'this->tb->' + code
                            rendered.append(code)

        return rendered
